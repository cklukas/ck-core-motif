/* Optional AT-SPI D-Bus adapter; all dispatch stays on the Xt UI thread.
 * LGPL-2.0-or-later. Protocol: at-spi2-core/xml/{Accessible,Application,Action,
 * Text,Table,Component,Value,Socket}.xml. No ATK, GLib, GTK or Qt is linked. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "AccessibleI.h"
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <dbus/dbus.h>
#include <locale.h>
#include <limits.h>
#ifdef HAVE_UNICODE_TEXT
#include "TextUnicode.h"
#include <unicode/ubrk.h>
#include <unicode/utext.h>
#include <unicode/utf8.h>
#include <unicode/uchar.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ROOT "/org/a11y/atspi/accessible/root"
#define PREFIX "/org/a11y/atspi/accessible"
#define NULLPATH "/org/a11y/atspi/null"
#define ACCESS "org.a11y.atspi.Accessible"
#define APP "org.a11y.atspi.Application"
#define ACTION "org.a11y.atspi.Action"
#define VALUE "org.a11y.atspi.Value"
#define TEXT "org.a11y.atspi.Text"
#define TABLE "org.a11y.atspi.Table"
#define SELECTION "org.a11y.atspi.Selection"
#define COMPONENT "org.a11y.atspi.Component"
#define CACHE "org.a11y.atspi.Cache"
#define CACHEPATH "/org/a11y/atspi/cache"
#define CACHE_ITEM "((so)(so)(so)iiassusau)"
typedef struct _TransportState {
  XmAccessibleId id;
  uint64_t states;
  unsigned int role;
  char *name, *value_text, *text_content;
  int text_length, caret;
  unsigned char text_known;
  double minimum, maximum, current, increment;
  unsigned char known, has_value;
  struct _TransportState *next;
} TransportState;
typedef struct {
  XmAccessibility *owner;
  DBusConnection *connection;
  DBusPendingCall *embed;
  XtInputId input;
  XtIntervalId timer;
  int application_id;
  Boolean key_pending;
  TransportState **memo;
  size_t memo_capacity, memo_count;
} Transport;
static const char *text(const char *s) { return s ? s : ""; }
static void path_for(XmAccessibleId id, char *buffer, size_t length) {
  if (id == 1)
    snprintf(buffer, length, "%s", ROOT);
  else if (id)
    snprintf(buffer, length, PREFIX "/%u", id);
  else
    snprintf(buffer, length, "%s", NULLPATH);
}
static void reference(DBusMessageIter *iter, Transport *t, XmAccessibleId id) {
  DBusMessageIter child;
  char buffer[96];
  const char *path = buffer;
  const char *bus = id ? dbus_bus_get_unique_name(t->connection) : "";
  path_for(id, buffer, sizeof(buffer));
  dbus_message_iter_open_container(iter, DBUS_TYPE_STRUCT, NULL, &child);
  dbus_message_iter_append_basic(&child, DBUS_TYPE_STRING, &bus);
  dbus_message_iter_append_basic(&child, DBUS_TYPE_OBJECT_PATH, &path);
  dbus_message_iter_close_container(iter, &child);
}
static void string_value(DBusMessageIter *i, const char *s) {
  const char *value = text(s);
  dbus_message_iter_append_basic(i, DBUS_TYPE_STRING, &value);
}
static int child_count(XmAccessibility *a, XmAccessibleId id) {
  return (int)_XmAccessibilityChildCount(a, id);
}
static XmAccessibleId child_at(XmAccessibility *a, XmAccessibleId id, int index) {
  return index < 0 ? 0 : _XmAccessibilityChildAt(a, id, (size_t)index);
}
static const char *role_name(unsigned int role) {
  switch (role) {
  case 75:
    return "application";
  case 23:
    return "frame";
  case 39:
    return "panel";
  case 29:
    return "label";
  case 43:
    return "push button";
  case 7:
    return "check box";
  case 61:
    return "text";
  case 40:
    return "password text";
  case 48:
    return "scroll bar";
  case 31:
    return "list";
  case 32:
    return "list item";
  case 33:
    return "menu";
  case 34:
    return "menu bar";
  case 35:
    return "menu item";
  case 8:
    return "check menu item";
  case 45:
    return "radio menu item";
  case 55:
    return "table";
  case 56:
    return "table cell";
  default:
    return "unknown";
  }
}
static void interfaces(DBusMessageIter *i, XmAccessibleInfo *info,
                       XmAccessibleId id) {
  DBusMessageIter array;
  dbus_message_iter_open_container(i, DBUS_TYPE_ARRAY, "s", &array);
  string_value(&array, ACCESS);
  string_value(&array, COMPONENT);
  if (id == 1)
    string_value(&array, APP);
  if (info->action_count)
    string_value(&array, ACTION);
  if (info->has_selection) string_value(&array, SELECTION);
  if (info->has_value)
    string_value(&array, VALUE);
  if (info->has_text)
    string_value(&array, TEXT);
  if (info->rows >= 0 && info->columns >= 0)
    string_value(&array, TABLE);
  dbus_message_iter_close_container(i, &array);
}
static void states(DBusMessageIter *i, uint64_t bits) {
  DBusMessageIter array;
  dbus_uint32_t lower = (dbus_uint32_t)bits,
                upper = (dbus_uint32_t)(bits >> 32);
  dbus_message_iter_open_container(i, DBUS_TYPE_ARRAY, "u", &array);
  dbus_message_iter_append_basic(&array, DBUS_TYPE_UINT32, &lower);
  dbus_message_iter_append_basic(&array, DBUS_TYPE_UINT32, &upper);
  dbus_message_iter_close_container(i, &array);
}
/* Only live nodes retain memo records. Monotonic IDs never imply a growing
 * dense allocation; removal shrinks the hash table as the live tree shrinks. */
static size_t memo_bucket(XmAccessibleId id, size_t capacity) {
  return ((size_t)id * (size_t)UINT32_C(2654435761)) & (capacity - 1);
}
static void memo_resize(Transport *t, size_t capacity) {
  TransportState **table;
  size_t k;
  table = (TransportState **)XtCalloc((Cardinal)capacity, sizeof(*table));
  for (k = 0; k < t->memo_capacity; ++k) {
    TransportState *record = t->memo[k];
    while (record) {
      TransportState *next = record->next;
      size_t bucket = memo_bucket(record->id, capacity);
      record->next = table[bucket]; table[bucket] = record; record = next;
    }
  }
  XtFree((char *)t->memo); t->memo = table; t->memo_capacity = capacity;
}
static TransportState *memo_get(Transport *t, XmAccessibleId id, int create) {
  TransportState *record;
  size_t bucket;
  if (!t->memo_capacity) {
    if (!create) return NULL;
    memo_resize(t, 64);
  }
  bucket = memo_bucket(id, t->memo_capacity);
  for (record = t->memo[bucket]; record; record = record->next)
    if (record->id == id) return record;
  if (!create) return NULL;
  if (t->memo_count >= t->memo_capacity && t->memo_capacity <= UINT_MAX / (2 * sizeof(*t->memo))) {
    memo_resize(t, t->memo_capacity * 2);
    bucket = memo_bucket(id, t->memo_capacity);
  }
  record = (TransportState *)XtCalloc(1, sizeof(*record));
  record->id = id; record->next = t->memo[bucket]; t->memo[bucket] = record;
  ++t->memo_count;
  return record;
}
static void memo_remove(Transport *t, XmAccessibleId id) {
  TransportState **link, *record;
  if (!t->memo_capacity) return;
  link = &t->memo[memo_bucket(id, t->memo_capacity)];
  while (*link && (*link)->id != id) link = &(*link)->next;
  if (!*link) return;
  record = *link; *link = record->next;
  XtFree(record->name); XtFree(record->value_text); XtFree(record->text_content); XtFree((char *)record); --t->memo_count;
  if (t->memo_capacity > 64 && t->memo_count < t->memo_capacity / 4)
    memo_resize(t, t->memo_capacity / 2);
}
static int index_in_parent(XmAccessibility *a, XmAccessibleNode *node) {
  return _XmAccessibilityIndexInParent(a, node->id);
}
static int cache_item(DBusMessageIter *out, Transport *t,
                       XmAccessibleId id) {
  XmAccessibleInfo info;
  XmAccessibleNode *node = _XmAccessibilityNode(t->owner, id);
  DBusMessageIter item;
  dbus_int32_t index, count;
  dbus_uint32_t role;
  if (!node || !XmAccessibilityQuery(t->owner, id, &info))
    return 0;
  if (!dbus_validate_utf8(text(info.name), NULL) ||
      !dbus_validate_utf8(text(info.description), NULL)) {
    XmAccessibleInfoClear(&info);
    return 0;
  }
  index = index_in_parent(t->owner, node);
  count = child_count(t->owner, id);
  role = info.role;
  dbus_message_iter_open_container(out, DBUS_TYPE_STRUCT, NULL, &item);
  reference(&item, t, id);
  reference(&item, t, 1);
  reference(&item, t, node->parent);
  dbus_message_iter_append_basic(&item, DBUS_TYPE_INT32, &index);
  dbus_message_iter_append_basic(&item, DBUS_TYPE_INT32, &count);
  interfaces(&item, &info, id);
  string_value(&item, info.name);
  dbus_message_iter_append_basic(&item, DBUS_TYPE_UINT32, &role);
  string_value(&item, info.description);
  states(&item, info.states);
  dbus_message_iter_close_container(out, &item);
  {
    TransportState *record = memo_get(t, id, 1);
    record->known = 1;
    XtFree(record->name); record->name = XtNewString(text(info.name));
  }
  XmAccessibleInfoClear(&info);
  return 1;
}
static void send_reply(Transport *t, DBusMessage *reply) {
  if (reply) {
    dbus_connection_send(t->connection, reply, NULL);
    dbus_message_unref(reply);
  }
}
static void error_reply(Transport *t, DBusMessage *call, const char *name,
                        const char *message) {
  send_reply(t, dbus_message_new_error(call, name, message));
}
static const char *property_signature(const char *iface, const char *name) {
  if (!strcmp(iface, ACCESS)) {
    if (!strcmp(name, "Name") || !strcmp(name, "Description") ||
        !strcmp(name, "Locale") || !strcmp(name, "AccessibleId"))
      return "s";
    if (!strcmp(name, "Parent"))
      return "(so)";
    if (!strcmp(name, "ChildCount"))
      return "i";
    if (!strcmp(name, "version"))
      return "u";
  }
  if (!strcmp(iface, APP)) {
    if (!strcmp(name, "Id"))
      return "i";
    if (!strcmp(name, "InterfaceVersion"))
      return "u";
    if (!strcmp(name, "ToolkitName") || !strcmp(name, "Version") ||
        !strcmp(name, "ToolkitVersion") || !strcmp(name, "AtspiVersion"))
      return "s";
  }
  if (!strcmp(iface, VALUE)) {
    if (!strcmp(name, "MinimumValue") || !strcmp(name, "MaximumValue") ||
        !strcmp(name, "CurrentValue") || !strcmp(name, "MinimumIncrement"))
      return "d";
    if (!strcmp(name, "Text"))
      return "s";
    if (!strcmp(name, "version"))
      return "u";
  }
  if (!strcmp(iface, SELECTION) && !strcmp(name, "NSelectedChildren")) return "i";
  if (!strcmp(iface, ACTION) && !strcmp(name, "NActions"))
    return "i";
  if (!strcmp(iface, TEXT) &&
      (!strcmp(name, "CharacterCount") || !strcmp(name, "CaretOffset")))
    return "i";
  if (!strcmp(iface, TABLE) &&
      (!strcmp(name, "NRows") || !strcmp(name, "NColumns") ||
       !strcmp(name, "NSelectedRows") || !strcmp(name, "NSelectedColumns")))
    return "i";
  return NULL;
}
static Boolean supports_interface(XmAccessibleId id,
                                  const XmAccessibleInfo *info,
                                  const char *iface) {
  if (!strcmp(iface, APP))
    return id == 1;
  if (!strcmp(iface, ACTION))
    return info->action_count > 0;
  if (!strcmp(iface, SELECTION)) return info->has_selection;
  if (!strcmp(iface, VALUE))
    return info->has_value;
  if (!strcmp(iface, TEXT))
    return info->has_text;
  if (!strcmp(iface, TABLE))
    return info->rows >= 0 && info->columns >= 0;
  return !strcmp(iface, ACCESS) || !strcmp(iface, COMPONENT);
}
static void remove_interface(char *xml, const char *iface) {
  char prefix[128];
  char *first, *last;
  snprintf(prefix, sizeof(prefix), "<interface name='%s'>", iface);
  first = strstr(xml, prefix);
  if (!first)
    return;
  last = strstr(first, "</interface>");
  if (last)
    memmove(first, last + 12, strlen(last + 12) + 1);
}
static void property_value(DBusMessageIter *out, Transport *t,
                           XmAccessibleNode *node, XmAccessibleInfo *info,
                           const char *iface, const char *name) {
  dbus_int32_t number = 0;
  dbus_uint32_t version = 1;
  if (!strcmp(iface, VALUE) && !strcmp(property_signature(iface, name), "d")) {
    double value = !strcmp(name, "MinimumValue")       ? info->minimum_value
                   : !strcmp(name, "MaximumValue")     ? info->maximum_value
                   : !strcmp(name, "MinimumIncrement") ? info->minimum_increment
                                                       : info->current_value;
    dbus_message_iter_append_basic(out, DBUS_TYPE_DOUBLE, &value);
    return;
  }
  if (!strcmp(name, "Parent")) {
    reference(out, t, node->parent);
    return;
  }
  if (!strcmp(name, "version") || !strcmp(name, "InterfaceVersion")) {
    dbus_message_iter_append_basic(out, DBUS_TYPE_UINT32, &version);
    return;
  }
  if (!strcmp(property_signature(iface, name), "s")) {
    const char *value = "";
    if (!strcmp(iface, VALUE) && !strcmp(name, "Text"))
      value = text(info->value_text);
    else if (!strcmp(name, "Name"))
      value = text(info->name);
    else if (!strcmp(name, "Description"))
      value = text(info->description);
    else if (!strcmp(name, "Locale"))
      value = text(setlocale(LC_MESSAGES, NULL));
    else if (!strcmp(name, "ToolkitName"))
      value = "Motif";
    else if (!strcmp(name, "AtspiVersion"))
      value = "2.1";
    else if (!strcmp(name, "ToolkitVersion") || !strcmp(name, "Version"))
      value = XmVERSION_STRING;
    string_value(out, value);
    return;
  }
  if (!strcmp(name, "ChildCount"))
    number = child_count(t->owner, node->id);
  else if (!strcmp(name, "NSelectedChildren"))
    number = XmAccessibilitySelectionCount(t->owner, node->id);
  else if (!strcmp(name, "Id"))
    number = t->application_id;
  else if (!strcmp(name, "NActions"))
    number = (dbus_int32_t)info->action_count;
  else if (!strcmp(name, "CharacterCount"))
    number = info->text_length;
  else if (!strcmp(name, "CaretOffset"))
    number = info->caret;
  else if (!strcmp(name, "NRows"))
    number = info->rows;
  else if (!strcmp(name, "NColumns"))
    number = info->columns;
  dbus_message_iter_append_basic(out, DBUS_TYPE_INT32, &number);
}
static const char introspection[] =
    "<node><interface name='org.a11y.atspi.Selection'>"
    "<property name='NSelectedChildren' type='i' access='read'/>"
    "<method name='GetSelectedChild'><arg direction='in' type='i'/><arg direction='out' type='(so)'/></method>"
    "<method name='SelectChild'><arg direction='in' type='i'/><arg direction='out' type='b'/></method>"
    "<method name='DeselectSelectedChild'><arg direction='in' type='i'/><arg direction='out' type='b'/></method>"
    "<method name='IsChildSelected'><arg direction='in' type='i'/><arg direction='out' type='b'/></method>"
    "<method name='SelectAll'><arg direction='out' type='b'/></method>"
    "<method name='ClearSelection'><arg direction='out' type='b'/></method>"
    "<method name='DeselectChild'><arg direction='in' type='i'/><arg direction='out' type='b'/></method>"
    "</interface><interface name='org.freedesktop.DBus.Introspectable'><method "
    "name='Introspect'><arg direction='out' type='s'/></method></interface>"
    "<interface name='org.freedesktop.DBus.Properties'><method name='Get'><arg "
    "direction='in' type='s'/><arg direction='in' type='s'/><arg "
    "direction='out' type='v'/></method><method name='GetAll'><arg "
    "direction='in' type='s'/><arg direction='out' "
    "type='a{sv}'/></method><method name='Set'><arg direction='in' "
    "type='s'/><arg direction='in' type='s'/><arg direction='in' "
    "type='v'/></method></interface>"
    "<interface name='org.a11y.atspi.Accessible'><method name='GetRole'><arg "
    "direction='out' type='u'/></method><method name='GetState'><arg "
    "direction='out' type='au'/></method><method name='GetChildren'><arg "
    "direction='out' type='a(so)'/></method><method "
    "name='GetChildAtIndex'><arg direction='in' type='i'/><arg direction='out' "
    "type='(so)'/></method><method name='GetInterfaces'><arg direction='out' "
    "type='as'/></method></interface>"
    "<interface name='org.a11y.atspi.Action'><method name='DoAction'><arg "
    "direction='in' type='i'/><arg direction='out' "
    "type='b'/></method></interface>"
    "<interface name='org.a11y.atspi.Component'><method name='GetExtents'><arg "
    "direction='in' type='u'/><arg direction='out' "
    "type='(iiii)'/></method><method name='GetPosition'><arg direction='in' "
    "type='u'/><arg direction='out' type='i'/><arg direction='out' "
    "type='i'/></method><method name='GetSize'><arg direction='out' "
    "type='i'/><arg direction='out' type='i'/></method><method "
    "name='GrabFocus'><arg "
    "direction='out' type='b'/></method></interface>"
    "<interface name='org.a11y.atspi.Value'><property name='version' type='u' "
    "access='read'/>"
    "<property name='MinimumValue' type='d' access='read'/><property "
    "name='MaximumValue' type='d' access='read'/>"
    "<property name='MinimumIncrement' type='d' access='read'/><property "
    "name='CurrentValue' type='d' access='readwrite'/>"
    "<property name='Text' type='s' access='read'/></interface>"
    "<interface name='org.a11y.atspi.Text'><method name='GetText'><arg "
    "direction='in' type='i'/><arg direction='in' type='i'/><arg "
    "direction='out' type='s'/></method><method "
    "name='GetStringAtOffset'><arg direction='in' type='i'/><arg direction='in' type='u'/>"
    "<arg direction='out' type='s'/><arg direction='out' type='i'/><arg direction='out' type='i'/></method><method "
    "name='GetTextAtOffset'><arg direction='in' type='i'/><arg direction='in' type='u'/>"
    "<arg direction='out' type='s'/><arg direction='out' type='i'/><arg direction='out' type='i'/></method><method "
    "name='GetTextBeforeOffset'><arg direction='in' type='i'/><arg direction='in' type='u'/>"
    "<arg direction='out' type='s'/><arg direction='out' type='i'/><arg direction='out' type='i'/></method><method "
    "name='GetTextAfterOffset'><arg direction='in' type='i'/><arg direction='in' type='u'/>"
    "<arg direction='out' type='s'/><arg direction='out' type='i'/><arg direction='out' type='i'/></method><method "
    "name='GetNSelections'><arg direction='out' type='i'/></method><method "
    "name='GetSelection'><arg direction='in' type='i'/><arg direction='out' type='i'/>"
    "<arg direction='out' type='i'/></method><method "
    "name='GetCharacterAtOffset'><arg direction='in' type='i'/><arg "
    "direction='out' type='i'/></method></interface>"
    "<interface name='org.a11y.atspi.Table'><method "
    "name='GetAccessibleAt'><arg direction='in' type='i'/><arg direction='in' "
    "type='i'/><arg direction='out' type='(so)'/></method></interface></node>";
static int line_separator_before(XmAccessibility *a, XmAccessibleId id, int end) {
  char *last;
  int result = 0;
  if (end <= 0) return 0;
  last = XmAccessibilityText(a, id, end - 1, end);
  if (!last) return 0;
  if (!strcmp(last, "\n") || !strcmp(last, "\r") ||
      !strcmp(last, "\xc2\x85") || !strcmp(last, "\xe2\x80\xa8") ||
      !strcmp(last, "\xe2\x80\xa9")) result = 1;
  if (!strcmp(last, "\n") && end > 1) {
    char *previous = XmAccessibilityText(a, id, end - 2, end - 1);
    if (previous && !strcmp(previous, "\r")) result = 2;
    XtFree(previous);
  }
  XtFree(last);
  return result;
}
/* Wire offsets are Unicode scalars. ICU UText break offsets are UTF-8 bytes;
 * the shared immutable map performs exact conversion between the two. Visual
 * line boundaries are supplied by the actual native/provider layout. */
static int text_range(XmAccessibility *a, XmAccessibleId id, int length,
                      int offset, unsigned int boundary, int modern,
                      int direction, int *first, int *last) {
  int line = modern ? boundary == 3 : boundary == 5 || boundary == 6;
  if (offset < 0 || offset > length || (modern ? boundary > 4 : boundary > 6))
    return -1;
  if (line) {
    int start, end;
    if (!modern && boundary == 6) {
      int shift = direction;
      /* Legacy LINE_END places hard separators at the beginning of the next
       * range. Soft-wrap boundaries stay unchanged. */
      if (!XmAccessibilityVisualLine(a, id, offset, 0, &start, &end)) return 0;
      if (start < end && offset >= end - line_separator_before(a, id, end) &&
          line_separator_before(a, id, end)) ++shift;
      if (shift > 1) {
        if (!XmAccessibilityVisualLine(a, id, offset, 1, &start, &end)) return 0;
        if (!XmAccessibilityVisualLine(a, id, start, 1, &start, &end)) return 0;
      } else if (!XmAccessibilityVisualLine(a, id, offset, shift, &start, &end)) return 0;
      if (start == end) { *first = *last = start; return 1; }
      *first = start - line_separator_before(a, id, start);
      *last = end - line_separator_before(a, id, end);
      return 1;
    }
    return XmAccessibilityVisualLine(a, id, offset, direction, first, last);
  }
  if (boundary == 0) {
    int position = offset + direction;
    if (position < 0) position = 0;
    if (position > length) position = length;
    *first = position;
    *last = position < length ? position + 1 : position;
    if (offset == 0 && direction < 0) *last = 0;
    return 1;
  }
#ifdef HAVE_UNICODE_TEXT
  {
    char *source = XmAccessibilityText(a, id, 0, length);
    XmTextUnicodeMap *map;
    UBreakIterator *iterator = NULL;
    UText *utext = NULL;
    UErrorCode status = U_ZERO_ERROR;
    int *breaks = NULL, count = 0, capacity = 0, selected, k, trailing_paragraph = 0;
    int32_t byte;
    size_t scalar, byte_length;
    int paragraph = modern && boundary == 4;
    if (!source || !dbus_validate_utf8(source, NULL)) {
      XtFree(source);
      return 0;
    }
    byte_length = strlen(source);
    map = XmTextUnicodeMapCreate(source, byte_length);
    if (!map || XmTextUnicodeScalarCount(map) != (size_t)length) {
      XmTextUnicodeMapFree(map); XtFree(source); return 0;
    }
#define ADD_BREAK(value) do { \
      int candidate = (value); \
      if (!count || breaks[count - 1] != candidate) { \
        if (count == capacity) { \
          int next; \
          if (capacity > INT_MAX / 2) goto range_failed; \
          next = capacity ? capacity * 2 : 32; \
          if ((size_t)next > UINT_MAX / sizeof(*breaks)) goto range_failed; \
          breaks = (int *)XtRealloc((char *)breaks, (Cardinal)((size_t)next * sizeof(*breaks))); \
          capacity = next; \
        } \
        breaks[count++] = candidate; \
      } \
    } while (0)
    ADD_BREAK(0);
    if (paragraph) {
      int32_t cursor = 0;
      while ((size_t)cursor < byte_length) {
        UChar32 codepoint;
        U8_NEXT(source, cursor, (int32_t)byte_length, codepoint);
        if (codepoint == '\r' && (size_t)cursor < byte_length && source[cursor] == '\n') ++cursor;
        if (codepoint == '\r' || codepoint == '\n' || codepoint == 0x2029) {
          if (!XmTextUnicodeScalarAtByte(map, (size_t)cursor, &scalar)) goto range_failed;
          ADD_BREAK((int)scalar);
        }
      }
    } else {
      UBreakIteratorType type = modern ? (boundary == 1 ? UBRK_WORD : UBRK_SENTENCE)
                                       : (boundary <= 2 ? UBRK_WORD : UBRK_SENTENCE);
      iterator = ubrk_open(type, "", NULL, 0, &status);
      utext = utext_openUTF8(NULL, source, (int64_t)byte_length, &status);
      if (U_FAILURE(status) || !iterator || !utext) goto range_failed;
      ubrk_setUText(iterator, utext, &status);
      if (U_FAILURE(status)) goto range_failed;
      byte = ubrk_first(iterator);
      while (byte != UBRK_DONE) {
        int32_t candidate = byte;
        if (!modern && type == UBRK_WORD) {
          int32_t next = ubrk_next(iterator);
          if (next == UBRK_DONE) break;
          if (ubrk_getRuleStatus(iterator) >= UBRK_WORD_NUMBER) {
            candidate = boundary == 1 ? byte : next;
            if (!XmTextUnicodeScalarAtByte(map, (size_t)candidate, &scalar)) goto range_failed;
            ADD_BREAK((int)scalar);
          }
          byte = next;
          continue;
        }
        if (!modern && boundary == 4 && candidate > 0) {
          int32_t cursor = candidate;
          UChar32 codepoint;
          while (cursor > 0) {
            int32_t previous = cursor;
            U8_PREV(source, 0, previous, codepoint);
            if (!u_isUWhiteSpace(codepoint)) break;
            cursor = previous;
          }
          candidate = cursor;
        }
        if (!XmTextUnicodeScalarAtByte(map, (size_t)candidate, &scalar)) goto range_failed;
        ADD_BREAK((int)scalar);
        byte = ubrk_next(iterator);
      }
    }
    ADD_BREAK(length);
    /* A trailing hard paragraph separator owns an empty final paragraph. */
    if (paragraph && byte_length && (source[byte_length - 1] == '\n' || source[byte_length - 1] == '\r' ||
        (byte_length >= 3 && !memcmp(source + byte_length - 3, "\xe2\x80\xa9", 3)))) {
      trailing_paragraph = 1;
    }
    selected = count - 2;
    if (selected < 0) selected = 0;
    for (k = 0; k + 1 < count; ++k)
      if (offset < breaks[k + 1]) { selected = k; break; }
    if (trailing_paragraph && offset == length) selected = count - 1;
    selected += direction;
    if (selected < 0) *first = *last = 0;
    else if (selected + 1 >= count) *first = *last = length;
    else { *first = breaks[selected]; *last = breaks[selected + 1]; }
    if (iterator) ubrk_close(iterator);
    if (utext) utext_close(utext);
    XmTextUnicodeMapFree(map); XtFree(source); XtFree((char *)breaks);
    return 1;
range_failed:
    if (iterator) ubrk_close(iterator);
    if (utext) utext_close(utext);
    XmTextUnicodeMapFree(map); XtFree(source); XtFree((char *)breaks);
    return 0;
#undef ADD_BREAK
  }
#else
  (void)a; (void)id; (void)direction; (void)first; (void)last;
  return 0;
#endif
}
static DBusHandlerResult dispatch_inner(DBusConnection *connection,
                                        DBusMessage *call, void *data) {
  Transport *t = (Transport *)data;
  XmAccessibility *a = t->owner;
  XmAccessibleNode *node;
  const char *path = dbus_message_get_path(call),
             *iface = dbus_message_get_interface(call),
             *method = dbus_message_get_member(call);
  XmAccessibleId id = 0;
  XmAccessibleInfo info;
  DBusMessage *reply;
  DBusMessageIter out, iter;
  dbus_int32_t index = 0, end = 0;
  dbus_bool_t success = False;
  dbus_uint32_t role;
  (void)connection;
  if (dbus_message_get_type(call) != DBUS_MESSAGE_TYPE_METHOD_CALL || !path ||
      !iface || !method)
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  if (!strcmp(path, CACHEPATH)) {
    DBusMessageIter array;
    XmAccessibleId *ids;
    size_t count = 0, k;
    if (!strcmp(iface, "org.freedesktop.DBus.Introspectable") &&
        !strcmp(method, "Introspect")) {
      const char *xml = "<node><interface name='org.a11y.atspi.Cache'>"
        "<method name='GetItems'><arg direction='out' type='a" CACHE_ITEM "'/></method>"
        "<signal name='AddAccessible'><arg type='" CACHE_ITEM "'/></signal>"
        "<signal name='RemoveAccessible'><arg type='(so)'/></signal></interface></node>";
      reply = dbus_message_new_method_return(call);
      dbus_message_iter_init_append(reply, &out);
      string_value(&out, xml);
      send_reply(t, reply);
      return DBUS_HANDLER_RESULT_HANDLED;
    }
    if (strcmp(iface, CACHE) || strcmp(method, "GetItems"))
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    if (dbus_message_get_signature(call)[0]) {
      error_reply(t, call, DBUS_ERROR_INVALID_ARGS, "GetItems takes no arguments");
      return DBUS_HANDLER_RESULT_HANDLED;
    }
    /* Snapshot identities before provider callbacks, which may remove nodes. */
    for (node = a->nodes; node; node = node->next) ++count;
    if (count > UINT_MAX / sizeof(*ids)) {
      error_reply(t, call, DBUS_ERROR_LIMITS_EXCEEDED, "Accessible tree is too large");
      return DBUS_HANDLER_RESULT_HANDLED;
    }
    ids = (XmAccessibleId *)XtMalloc((Cardinal)(count * sizeof(*ids)));
    if (!ids) {
      error_reply(t, call, "org.freedesktop.DBus.Error.NoMemory",
                  "Unable to snapshot the accessibility tree");
      return DBUS_HANDLER_RESULT_HANDLED;
    }
    for (node = a->nodes, k = 0; node; node = node->next) ids[k++] = node->id;
    reply = dbus_message_new_method_return(call);
    dbus_message_iter_init_append(reply, &out);
    dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, CACHE_ITEM, &array);
    for (k = 0; k < count; ++k) cache_item(&array, t, ids[k]);
    dbus_message_iter_close_container(&out, &array);
    XtFree((char *)ids);
    send_reply(t, reply);
    return DBUS_HANDLER_RESULT_HANDLED;
  }
  if (!strcmp(path, ROOT))
    id = 1;
  else if (!strncmp(path, PREFIX "/", sizeof(PREFIX))) {
    char *tail = NULL;
    unsigned long parsed = strtoul(path + sizeof(PREFIX), &tail, 10);
    if (tail && !*tail && parsed <= UINT32_MAX)
      id = (XmAccessibleId)parsed;
  }
  node = _XmAccessibilityNode(a, id);
  if (!node || !XmAccessibilityQuery(a, id, &info)) {
    error_reply(t, call, "org.freedesktop.DBus.Error.UnknownObject",
                "Accessible object is gone");
    return DBUS_HANDLER_RESULT_HANDLED;
  }
  if (!dbus_validate_utf8(text(info.name), NULL) ||
      !dbus_validate_utf8(text(info.description), NULL) ||
      !dbus_validate_utf8(text(info.value_text), NULL)) {
    XmAccessibleInfoClear(&info);
    error_reply(t, call, DBUS_ERROR_FAILED, "Provider returned invalid UTF-8");
    return DBUS_HANDLER_RESULT_HANDLED;
  }
  reply = dbus_message_new_method_return(call);
  dbus_message_iter_init_append(reply, &out);
  if (!strcmp(iface, "org.freedesktop.DBus.Introspectable") &&
      !strcmp(method, "Introspect")) {
    char *xml = XtNewString(introspection);
    if (!supports_interface(id, &info, ACTION))
      remove_interface(xml, ACTION);
    if (!supports_interface(id, &info, TEXT))
      remove_interface(xml, TEXT);
    if (!supports_interface(id, &info, VALUE))
      remove_interface(xml, VALUE);
    if (!supports_interface(id, &info, SELECTION)) remove_interface(xml, SELECTION);
    if (!supports_interface(id, &info, TABLE))
      remove_interface(xml, TABLE);
    string_value(&out, xml);
    XtFree(xml);
    goto done;
  }
  if (!strcmp(iface, "org.freedesktop.DBus.Properties")) {
    const char *requested = NULL, *name = NULL, *signature;
    DBusMessageIter variant;
    if (!strcmp(method, "Set")) {
      if (dbus_message_iter_init(call, &iter) &&
          dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRING) {
        dbus_message_iter_get_basic(&iter, &requested);
        dbus_message_iter_next(&iter);
        if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRING) {
          dbus_message_iter_get_basic(&iter, &name);
          dbus_message_iter_next(&iter);
          if (!strcmp(requested, VALUE) && info.has_value &&
              !strcmp(name, "CurrentValue") &&
              dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_VARIANT) {
            double value;
            dbus_message_iter_recurse(&iter, &variant);
            if (dbus_message_iter_get_arg_type(&variant) != DBUS_TYPE_DOUBLE)
              goto invalid;
            dbus_message_iter_get_basic(&variant, &value);
            if (!XmAccessibilitySetValue(a, id, value)) {
              dbus_message_unref(reply);
              XmAccessibleInfoClear(&info);
              error_reply(t, call, DBUS_ERROR_FAILED,
                          "Value is read-only or rejected by its owner");
              return DBUS_HANDLER_RESULT_HANDLED;
            }
            goto done;
          }
          if (id == 1 && !strcmp(requested, APP) && !strcmp(name, "Id") &&
              dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_VARIANT) {
            dbus_message_iter_recurse(&iter, &variant);
            if (dbus_message_iter_get_arg_type(&variant) == DBUS_TYPE_INT32) {
              dbus_message_iter_get_basic(&variant, &t->application_id);
              goto done;
            }
          }
        }
      }
      goto invalid;
    }
    if (!strcmp(method, "Get")) {
      if (!dbus_message_get_args(call, NULL, DBUS_TYPE_STRING, &requested,
                                 DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID))
        goto invalid;
      if (!supports_interface(id, &info, requested))
        goto unsupported;
      signature = property_signature(requested, name);
      if (!signature)
        goto unsupported;
      if (!strcmp(requested, SELECTION)) {
        dbus_int32_t selected = XmAccessibilitySelectionCount(a, id);
        if (selected < 0 || _XmAccessibilityNode(a, id) != node || a->destroy_requested) goto unsupported;
        dbus_message_iter_open_container(&out, DBUS_TYPE_VARIANT, "i", &variant);
        dbus_message_iter_append_basic(&variant, DBUS_TYPE_INT32, &selected);
        dbus_message_iter_close_container(&out, &variant); goto done;
      }
      dbus_message_iter_open_container(&out, DBUS_TYPE_VARIANT, signature,
                                       &variant);
      property_value(&variant, t, node, &info, requested, name);
      dbus_message_iter_close_container(&out, &variant);
      goto done;
    }
    if (!strcmp(method, "GetAll")) {
      const char *names[] = {"MinimumValue",
                             "MaximumValue",
                             "CurrentValue",
                             "MinimumIncrement",
                             "Text",
                             "Name",
                             "Description",
                             "Parent",
                             "ChildCount",
                             "Locale",
                             "AccessibleId",
                             "version",
                             "Id",
                             "InterfaceVersion",
                             "ToolkitName",
                             "ToolkitVersion",
                             "Version",
                             "AtspiVersion",
                             "NActions",
                             "CharacterCount",
                             "CaretOffset",
                             "NRows",
                             "NColumns",
                             "NSelectedRows",
                             "NSelectedColumns", "NSelectedChildren"};
      DBusMessageIter array, entry;
      size_t i;
      if (!dbus_message_get_args(call, NULL, DBUS_TYPE_STRING, &requested,
                                 DBUS_TYPE_INVALID))
        goto invalid;
      if (!supports_interface(id, &info, requested))
        goto unsupported;
      if (!strcmp(requested, SELECTION)) {
        dbus_int32_t selected = XmAccessibilitySelectionCount(a, id);
        if (selected < 0 || _XmAccessibilityNode(a, id) != node || a->destroy_requested) goto unsupported;
        dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "{sv}", &array);
        dbus_message_iter_open_container(&array, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
        string_value(&entry, "NSelectedChildren");
        dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "i", &variant);
        dbus_message_iter_append_basic(&variant, DBUS_TYPE_INT32, &selected);
        dbus_message_iter_close_container(&entry, &variant);
        dbus_message_iter_close_container(&array, &entry);
        dbus_message_iter_close_container(&out, &array); goto done;
      }
      dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "{sv}", &array);
      for (i = 0; i < sizeof(names) / sizeof(names[0]); i++)
        if ((signature = property_signature(requested, names[i]))) {
          dbus_message_iter_open_container(&array, DBUS_TYPE_DICT_ENTRY, NULL,
                                           &entry);
          string_value(&entry, names[i]);
          dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, signature,
                                           &variant);
          property_value(&variant, t, node, &info, requested, names[i]);
          dbus_message_iter_close_container(&entry, &variant);
          dbus_message_iter_close_container(&array, &entry);
        }
      dbus_message_iter_close_container(&out, &array);
      goto done;
    }
    goto unsupported;
  }
  if (!strcmp(iface, SELECTION) && info.has_selection) {
    XmAccessibleId child = 0;
    dbus_bool_t accepted = FALSE;
    if (!strcmp(method, "SelectAll") || !strcmp(method, "ClearSelection")) {
      if (!dbus_message_has_signature(call, "")) goto invalid;
      accepted = XmAccessibilityChangeSelection(a, id,
          !strcmp(method, "SelectAll") ? XmACCESSIBLE_SELECT_ALL : XmACCESSIBLE_CLEAR_SELECTION, 0);
    } else {
      if (!dbus_message_get_args(call, NULL, DBUS_TYPE_INT32, &index, DBUS_TYPE_INVALID) || index < 0) goto invalid;
      if (!strcmp(method, "GetSelectedChild")) {
        child = XmAccessibilitySelectedChild(a, id, index);
        if (!child) goto invalid;
        reference(&out, t, child); goto done;
      }
      child = !strcmp(method, "DeselectSelectedChild") ? XmAccessibilitySelectedChild(a, id, index)
                                                       : _XmAccessibilityChildAt(a, id, (size_t)index);
      if (!child) goto invalid;
      if (!strcmp(method, "IsChildSelected")) accepted = XmAccessibilityChildSelected(a, id, child);
      else if (!strcmp(method, "SelectChild")) accepted = XmAccessibilityChangeSelection(a, id, XmACCESSIBLE_SELECT_CHILD, child);
      else if (!strcmp(method, "DeselectChild") || !strcmp(method, "DeselectSelectedChild"))
        accepted = XmAccessibilityChangeSelection(a, id, XmACCESSIBLE_DESELECT_CHILD, child);
      else goto unsupported;
    }
    dbus_message_iter_append_basic(&out, DBUS_TYPE_BOOLEAN, &accepted); goto done;
  }
  if (!strcmp(iface, ACCESS)) {
    if (!strcmp(method, "GetRole")) {
      role = info.role;
      dbus_message_iter_append_basic(&out, DBUS_TYPE_UINT32, &role);
      goto done;
    }
    if (!strcmp(method, "GetRoleName") ||
        !strcmp(method, "GetLocalizedRoleName")) {
      string_value(&out, role_name(info.role));
      goto done;
    }
    if (!strcmp(method, "GetState")) {
      states(&out, info.states);
      goto done;
    }
    if (!strcmp(method, "GetInterfaces")) {
      interfaces(&out, &info, id);
      goto done;
    }
    if (!strcmp(method, "GetApplication")) {
      reference(&out, t, 1);
      goto done;
    }
    if (!strcmp(method, "GetChildAtIndex")) {
      if (!dbus_message_get_args(call, NULL, DBUS_TYPE_INT32, &index,
                                 DBUS_TYPE_INVALID) ||
          index < 0)
        goto invalid;
      reference(&out, t, child_at(a, id, index));
      goto done;
    }
    if (!strcmp(method, "GetChildren")) {
      DBusMessageIter array;
      int i, count = child_count(a, id);
      dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "(so)", &array);
      for (i = 0; i < count; i++)
        reference(&array, t, child_at(a, id, i));
      dbus_message_iter_close_container(&out, &array);
      goto done;
    }
    if (!strcmp(method, "GetIndexInParent")) {
      index = index_in_parent(a, node);
      dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &index);
      goto done;
    }
    if (!strcmp(method, "GetRelationSet")) {
      DBusMessageIter array;
      dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "(ua(so))",
                                       &array);
      dbus_message_iter_close_container(&out, &array);
      goto done;
    }
    if (!strcmp(method, "GetAttributes")) {
      DBusMessageIter array;
      dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "{ss}", &array);
      dbus_message_iter_close_container(&out, &array);
      goto done;
    }
  }
  if (!strcmp(iface, APP) && id == 1) {
    if (!strcmp(method, "GetLocale")) {
      string_value(&out, setlocale(LC_MESSAGES, NULL));
      goto done;
    }
    if (!strcmp(method, "GetApplicationBusAddress")) {
      string_value(&out, "");
      goto done;
    }
  }
  if (!strcmp(iface, ACTION) && info.action_count) {
    if (!dbus_message_get_args(call, NULL, DBUS_TYPE_INT32, &index,
                               DBUS_TYPE_INVALID) ||
        index < 0 || (unsigned int)index >= info.action_count)
      goto invalid;
    if (!strcmp(method, "DoAction")) {
      success = XmAccessibilityAction(a, id, (unsigned int)index);
      dbus_message_iter_append_basic(&out, DBUS_TYPE_BOOLEAN, &success);
      goto done;
    }
    if (!strcmp(method, "GetName") || !strcmp(method, "GetLocalizedName") ||
        !strcmp(method, "GetDescription")) {
      char *name = XmAccessibilityActionName(a, id, (unsigned int)index);
      if (!dbus_validate_utf8(text(name), NULL)) {
        XtFree(name);
        goto invalid;
      }
      string_value(&out, name);
      XtFree(name);
      goto done;
    }
    if (!strcmp(method, "GetKeyBinding")) {
      string_value(&out, "");
      goto done;
    }
  }
  if (!strcmp(iface, COMPONENT) &&
      (!strcmp(method, "GetExtents") || !strcmp(method, "GetPosition") ||
       !strcmp(method, "GetSize"))) {
    dbus_uint32_t coordinates = 0;
    dbus_int32_t x = info.screen_x, y = info.screen_y, width = info.width,
                 height = info.height;
    DBusMessageIter structure;
    XmAccessibleInfo parent_info;
    if (!info.has_extents)
      goto unsupported;
    if (strcmp(method, "GetSize")) {
      if (!dbus_message_get_args(call, NULL, DBUS_TYPE_UINT32, &coordinates,
                                 DBUS_TYPE_INVALID) ||
          coordinates > 2)
        goto invalid;
      if (coordinates) {
        XmAccessibleNode *origin = node;
        if (coordinates == 1) {
          while (origin && (!origin->widget || !XtIsShell(origin->widget)))
            origin = _XmAccessibilityNode(a, origin->parent);
        } else
          origin = _XmAccessibilityNode(a, node->parent);
        if (!origin || !XmAccessibilityQuery(a, origin->id, &parent_info))
          goto unsupported;
        /* A provider read may remove the target while leaving its parent
         * alive. Dispatch pins storage, but a retired target has no geometry. */
        if (!parent_info.has_extents || _XmAccessibilityNode(a, id) != node) {
          XmAccessibleInfoClear(&parent_info);
          goto unsupported;
        }
        {
          int64_t relative_x = (int64_t)x - parent_info.screen_x;
          int64_t relative_y = (int64_t)y - parent_info.screen_y;
          XmAccessibleInfoClear(&parent_info);
          if (relative_x < INT32_MIN || relative_x > INT32_MAX ||
              relative_y < INT32_MIN || relative_y > INT32_MAX)
            goto unsupported;
          x = (dbus_int32_t)relative_x;
          y = (dbus_int32_t)relative_y;
        }
      }
    }
    if (!strcmp(method, "GetExtents")) {
      dbus_message_iter_open_container(&out, DBUS_TYPE_STRUCT, NULL,
                                       &structure);
      dbus_message_iter_append_basic(&structure, DBUS_TYPE_INT32, &x);
      dbus_message_iter_append_basic(&structure, DBUS_TYPE_INT32, &y);
      dbus_message_iter_append_basic(&structure, DBUS_TYPE_INT32, &width);
      dbus_message_iter_append_basic(&structure, DBUS_TYPE_INT32, &height);
      dbus_message_iter_close_container(&out, &structure);
    } else if (!strcmp(method, "GetPosition")) {
      dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &x);
      dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &y);
    } else {
      dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &width);
      dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &height);
    }
    goto done;
  }
  if (!strcmp(iface, COMPONENT) && !strcmp(method, "GrabFocus")) {
    success = XmAccessibilityFocus(a, id);
    dbus_message_iter_append_basic(&out, DBUS_TYPE_BOOLEAN, &success);
    goto done;
  }
  if (!strcmp(iface, TEXT) && info.has_text) {
    if (!strcmp(method, "GetStringAtOffset") || !strcmp(method, "GetTextAtOffset") ||
        !strcmp(method, "GetTextBeforeOffset") || !strcmp(method, "GetTextAfterOffset")) {
      dbus_uint32_t boundary;
      dbus_int32_t first, last;
      int result, direction = !strcmp(method, "GetTextBeforeOffset") ? -1 :
                              (!strcmp(method, "GetTextAfterOffset") ? 1 : 0);
      char *slice;
      if (!dbus_message_get_args(call, NULL, DBUS_TYPE_INT32, &index,
            DBUS_TYPE_UINT32, &boundary, DBUS_TYPE_INVALID)) goto invalid;
      result = text_range(a, id, info.text_length, index, boundary,
                           !strcmp(method, "GetStringAtOffset"), direction, &first, &last);
      if (result < 0) goto invalid;
      if (!result) goto unsupported;
      slice = XmAccessibilityText(a, id, first, last);
      if (!slice || !dbus_validate_utf8(slice, NULL)) { XtFree(slice); goto invalid; }
      string_value(&out, slice); XtFree(slice);
      dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &first);
      dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &last);
      goto done;
    }
    if (!strcmp(method, "GetText")) {
      char *slice;
      if (!dbus_message_get_args(call, NULL, DBUS_TYPE_INT32, &index,
                                 DBUS_TYPE_INT32, &end, DBUS_TYPE_INVALID))
        goto invalid;
      if (end == -1)
        end = info.text_length;
      slice = XmAccessibilityText(a, id, index, end);
      if (!slice || !dbus_validate_utf8(slice, NULL)) {
        XtFree(slice);
        goto invalid;
      }
      string_value(&out, slice);
      XtFree(slice);
      goto done;
    }
    if (!strcmp(method, "GetCharacterAtOffset")) {
      const unsigned char *p;
      char *slice;
      dbus_int32_t scalar;
      if (!dbus_message_get_args(call, NULL, DBUS_TYPE_INT32, &index,
                                 DBUS_TYPE_INVALID) ||
          index < 0 || index >= info.text_length)
        goto invalid;
      slice = XmAccessibilityText(a, id, index, index + 1);
      if (!slice || !dbus_validate_utf8(slice, NULL) || !slice[0]) {
        XtFree(slice);
        goto invalid;
      }
      p = (const unsigned char *)slice;
      scalar = *p++;
      if (scalar >= 0xc0) {
        unsigned int remaining = scalar < 0xe0 ? 1 : (scalar < 0xf0 ? 2 : 3);
        scalar &= (1 << (6 - remaining)) - 1;
        while (remaining--)
          scalar = (scalar << 6) | (*p++ & 63);
      }
      XtFree(slice);
      dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &scalar);
      goto done;
    }
    if (!strcmp(method, "GetNSelections") || !strcmp(method, "GetSelection")) {
      Boolean selected;
      int first, last;
      if (!_XmAccessibilityNativeTextSelection(a, id, &selected, &first, &last))
        goto unsupported;
      if (!strcmp(method, "GetNSelections")) {
        index = selected ? 1 : 0;
        dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &index);
      } else {
        if (!dbus_message_get_args(call, NULL, DBUS_TYPE_INT32, &index,
              DBUS_TYPE_INVALID) || index != 0 || !selected) goto invalid;
        index = first; end = last;
        dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &index);
        dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &end);
      }
      goto done;
    }
  }
  if (!strcmp(iface, TABLE) && node->provider.cell &&
      !strcmp(method, "GetAccessibleAt")) {
    if (!dbus_message_get_args(call, NULL, DBUS_TYPE_INT32, &index,
                               DBUS_TYPE_INT32, &end, DBUS_TYPE_INVALID) ||
        index < 0 || end < 0 || index >= info.rows || end >= info.columns)
      goto invalid;
    reference(&out, t, node->provider.cell(node->data, index, end));
    goto done;
  }
unsupported:
  dbus_message_unref(reply);
  XmAccessibleInfoClear(&info);
  error_reply(t, call, DBUS_ERROR_NOT_SUPPORTED,
              "Method or property is not implemented by this provider");
  return DBUS_HANDLER_RESULT_HANDLED;
invalid:
  dbus_message_unref(reply);
  XmAccessibleInfoClear(&info);
  error_reply(t, call, DBUS_ERROR_INVALID_ARGS,
              "Invalid accessible method arguments");
  return DBUS_HANDLER_RESULT_HANDLED;
done:
  XmAccessibleInfoClear(&info);
  send_reply(t, reply);
  return DBUS_HANDLER_RESULT_HANDLED;
}
static DBusHandlerResult dispatch(DBusConnection *connection,
                                  DBusMessage *message, void *data) {
  Transport *t = (Transport *)data;
  DBusHandlerResult result;
  ++t->owner->dispatch_depth;
  result = dispatch_inner(connection, message, data);
  --t->owner->dispatch_depth;
  _XmAccessibilityDispatchDone(t->owner);
  return result;
}
static void pump(Transport *t) {
  unsigned int count = 0;
  if (!dbus_connection_read_write(t->connection, 0)) {
    t->owner->status = XmACCESSIBILITY_DISCONNECTED;
    if (t->input) {
      XtRemoveInput(t->input);
      t->input = 0;
    }
    return;
  }
  while (count++ < 64 && dbus_connection_dispatch(t->connection) ==
                             DBUS_DISPATCH_DATA_REMAINS) {
  }
}
/* Toolkit listeners are distinct from global X11/compositor listeners. This
 * pre-action handshake lets a listener observe/consume the actual native key,
 * and permits its accessibility queries while withholding further X events.
 * A missing listener/service, malformed reply or timeout always fails open. */
static uint64_t monotonic_milliseconds(void) {
  struct timespec now;
  if (clock_gettime(CLOCK_MONOTONIC, &now)) return 0;
  return (uint64_t)now.tv_sec * 1000 + (uint64_t)now.tv_nsec / 1000000;
}
Boolean _XmAccessibilityTransportKey(XmAccessibility *a, const XKeyEvent *event,
                                      KeySym symbol, const char *label,
                                      Boolean is_text) {
  Transport *t = (Transport *)a->transport;
  DBusMessage *call = NULL, *reply = NULL;
  DBusPendingCall *pending = NULL;
  DBusMessageIter out, item;
  dbus_uint32_t type, stamp_bits;
  dbus_int32_t identity, hardware, modifiers, stamp;
  dbus_bool_t printable = is_text, consumed = FALSE;
  uint64_t deadline, now;
  int item_open = 0;
  if (!t || a->status != XmACCESSIBILITY_CONNECTED || t->key_pending ||
      a->destroy_requested || !dbus_validate_utf8(label, NULL)) return False;
  now = monotonic_milliseconds();
  if (!now) return False;
  deadline = now + 500;
  t->key_pending = True;
  type = event->type == KeyPress ? 0 : 1;
  identity = (dbus_int32_t)symbol;
  /* Registry input and listener output are asymmetric: registryd's actual
   * device-event parser accepts signed 32-bit hardware/modifier fields, even
   * though its introspection and listener marshaller advertise unsigned ones.
   * X11 keycodes and the 16-bit modifier mask fit exactly in the input types. */
  hardware = (dbus_int32_t)event->keycode;
  modifiers = (dbus_int32_t)(event->state & 0xffff);
  stamp_bits = (dbus_uint32_t)event->time;
  memcpy(&stamp, &stamp_bits, sizeof(stamp));
  call = dbus_message_new_method_call("org.a11y.atspi.Registry",
      "/org/a11y/atspi/registry/deviceeventcontroller",
      "org.a11y.atspi.DeviceEventController", "NotifyListenersSync");
  if (!call) goto done;
  dbus_message_iter_init_append(call, &out);
  if (!dbus_message_iter_open_container(&out, DBUS_TYPE_STRUCT, NULL, &item))
    goto done;
  item_open = 1;
  if (!dbus_message_iter_append_basic(&item, DBUS_TYPE_UINT32, &type) ||
      !dbus_message_iter_append_basic(&item, DBUS_TYPE_INT32, &identity) ||
      !dbus_message_iter_append_basic(&item, DBUS_TYPE_INT32, &hardware) ||
      !dbus_message_iter_append_basic(&item, DBUS_TYPE_INT32, &modifiers) ||
      !dbus_message_iter_append_basic(&item, DBUS_TYPE_INT32, &stamp) ||
      !dbus_message_iter_append_basic(&item, DBUS_TYPE_STRING, &label) ||
      !dbus_message_iter_append_basic(&item, DBUS_TYPE_BOOLEAN, &printable) ||
      !dbus_message_iter_close_container(&out, &item)) goto done;
  item_open = 0;
  if (!dbus_connection_send_with_reply(t->connection, call, &pending, 500) ||
      !pending) goto done;
  while (!dbus_pending_call_get_completed(pending) && !a->destroy_requested) {
    unsigned int count = 0;
    now = monotonic_milliseconds();
    if (!now || now >= deadline || !dbus_connection_get_is_connected(t->connection))
      break;
    if (!dbus_connection_read_write(t->connection,
           deadline - now < 10 ? (int)(deadline - now) : 10)) break;
    while (count++ < 64 && !a->destroy_requested &&
        dbus_connection_dispatch(t->connection) == DBUS_DISPATCH_DATA_REMAINS) {}
  }
  if (!a->destroy_requested && dbus_pending_call_get_completed(pending)) {
    reply = dbus_pending_call_steal_reply(pending);
    if (!reply || dbus_message_get_type(reply) != DBUS_MESSAGE_TYPE_METHOD_RETURN ||
        !dbus_message_get_args(reply, NULL, DBUS_TYPE_BOOLEAN, &consumed,
                               DBUS_TYPE_INVALID)) consumed = FALSE;
  }
done:
  /* A reentrant action may request deferred bridge destruction. Give its
   * already-queued reply one nonblocking write opportunity before teardown;
   * never run more widget/X callbacks or wait for a late consume response. */
  if (a->destroy_requested) (void)dbus_connection_read_write(t->connection, 0);
  if (pending) {
    if (!dbus_pending_call_get_completed(pending)) dbus_pending_call_cancel(pending);
    dbus_pending_call_unref(pending);
  }
  if (item_open) dbus_message_iter_abandon_container(&out, &item);
  if (reply) dbus_message_unref(reply);
  if (call) dbus_message_unref(call);
  t->key_pending = False;
  return consumed ? True : False;
}
static void input_ready(XtPointer data, int *fd, XtInputId *id) {
  (void)fd;
  (void)id;
  pump((Transport *)data);
}
static void timer_ready(XtPointer data, XtIntervalId *id) {
  Transport *t = (Transport *)data;
  (void)id;
  t->timer = 0;
  pump(t);
  if (t->owner->root && t->owner->status != XmACCESSIBILITY_DISCONNECTED)
    t->timer = XtAppAddTimeOut(XtWidgetToApplicationContext(t->owner->root),
                               100, timer_ready, data);
}
static void state_event(Transport *, const char *, const char *, int);
static void window_event(Transport *, XmAccessibleId, const char *, int);
/* One current snapshot per live text node; no edit history is retained.
 * Read bounded scalar ranges, validate provider output, and abandon an observation
 * if a callback changes/removes the node. Unknown observations never fabricate
 * insertion/deletion events. */
static size_t scalar_bytes(const char *p) {
  unsigned char c = (unsigned char)*p;
  return c < 0x80 ? 1 : c < 0xe0 ? 2 : c < 0xf0 ? 3 : 4;
}
static char *text_snapshot(XmAccessibility *a, XmAccessibleId id, int length,
                           XmAccessibleNode *node, uint64_t serial) {
  char *result;
  size_t used = 0, capacity;
  int start;
  if (length < 0 || (size_t)length > (UINT_MAX - 1) / 4) return NULL;
  capacity = (size_t)length * 4 + 1;
  result = XtMalloc((Cardinal)capacity);
  if (!result)
    return NULL;
  for (start = 0; start < length;) {
    int end = length - start > 1024 ? start + 1024 : length;
    char *part = XmAccessibilityText(a, id, start, end), *p;
    int scalars = 0;
    size_t bytes;
    if (!part || _XmAccessibilityNode(a, id) != node ||
        a->destroy_requested || node->change_serial != serial ||
        !dbus_validate_utf8(part, NULL)) { XtFree(part); goto failed; }
    bytes = strlen(part);
    for (p = part; *p; p += scalar_bytes(p)) ++scalars;
    if (scalars != end - start || bytes >= capacity - used) {
      XtFree(part); goto failed;
    }
    memcpy(result + used, part, bytes); used += bytes;
    XtFree(part); start = end;
  }
  result[used] = 0;
  return result;
failed:
  XtFree(result);
  return NULL;
}
/* Construct the entire event before queueing it. A failed container open must
 * never be followed by operations on its uninitialized child iterator. */
static DBusMessage *text_signal(const char *path, const char *member,
                               const char *detail, int offset, int length,
                               const char *payload) {
  DBusMessage *message = dbus_message_new_signal(path,
      "org.a11y.atspi.Event.Object", member);
  DBusMessageIter out, variant, array;
  dbus_int32_t first = offset, second = length, zero = 0;
  int variant_open = 0, array_open = 0;
  if (!message) return NULL;
  dbus_message_iter_init_append(message, &out);
  if (!dbus_message_iter_append_basic(&out, DBUS_TYPE_STRING, &detail) ||
      !dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &first) ||
      !dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &second) ||
      !dbus_message_iter_open_container(&out, DBUS_TYPE_VARIANT,
                                        payload ? "s" : "i", &variant))
    goto failed;
  variant_open = 1;
  if (!(payload ? dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING,
                                                 &payload)
                : dbus_message_iter_append_basic(&variant, DBUS_TYPE_INT32,
                                                 &zero)) ||
      !dbus_message_iter_close_container(&out, &variant))
    goto failed;
  variant_open = 0;
  if (!dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "{sv}", &array))
    goto failed;
  array_open = 1;
  if (!dbus_message_iter_close_container(&out, &array)) goto failed;
  return message;
failed:
  if (variant_open) dbus_message_iter_abandon_container(&out, &variant);
  if (array_open) dbus_message_iter_abandon_container(&out, &array);
  dbus_message_unref(message);
  return NULL;
}
static void sync_text(Transport *t, XmAccessibleId id,
                      const XmAccessibleInfo *info, int notify) {
  XmAccessibility *a = t->owner;
  XmAccessibleNode *node = _XmAccessibilityNode(a, id);
  TransportState *record;
  char *current = NULL, *previous;
  char path[96];
  uint64_t serial;
  int known, old_length, old_caret;
  DBusMessage *events[4] = {NULL, NULL, NULL, NULL};
  size_t event_count = 0, k;
  int events_ok = 1;
  if (!node || node->text_snapshot_busy) return;
  serial = node->change_serial;
  node->text_snapshot_busy = True;
  if (info->has_text && info->role != XmACCESSIBLE_PASSWORD_TEXT)
    current = text_snapshot(a, id, info->text_length, node, serial);
  if (_XmAccessibilityNode(a, id) != node || a->destroy_requested) {
    XtFree(current); return;
  }
  node->text_snapshot_busy = False;
  if (node->change_serial != serial) { XtFree(current); current = NULL; }
  record = memo_get(t, id, 1);
  previous = record->text_content;
  known = record->text_known;
  old_length = record->text_length; old_caret = record->caret;
  record->text_content = current;
  record->text_known = current != NULL;
  record->text_length = info->text_length; record->caret = info->caret;
  path_for(id, path, sizeof(path));
  if (notify && known && current) {
    char *old_end = previous + strlen(previous), *new_end = current + strlen(current);
    char *old_start = previous, *new_start = current;
    int prefix = 0, suffix = 0;
    while (*old_start && *new_start) {
      size_t bytes = scalar_bytes(old_start);
      if (bytes != scalar_bytes(new_start) || memcmp(old_start, new_start, bytes)) break;
      old_start += bytes; new_start += bytes; ++prefix;
    }
    while (old_end > old_start && new_end > new_start) {
      char *op = old_end - 1, *np = new_end - 1;
      while (((unsigned char)*op & 0xc0) == 0x80) --op;
      while (((unsigned char)*np & 0xc0) == 0x80) --np;
      if (old_end - op != new_end - np || memcmp(op, np, (size_t)(old_end-op))) break;
      old_end = op; new_end = np; ++suffix;
    }
    if (old_end != old_start || new_end != new_start) {
      char saved = *old_end;
      *old_end = 0;
      if (old_end != old_start) events[event_count++] = text_signal(path,
          "TextChanged", "delete", prefix, old_length - prefix - suffix, old_start);
      *old_end = saved; saved = *new_end; *new_end = 0;
      if (new_end != new_start) events[event_count++] = text_signal(path,
          "TextChanged", "insert", prefix, info->text_length - prefix - suffix, new_start);
      *new_end = saved;
      events[event_count++] = text_signal(path, "VisibleDataChanged", "", 0, 0, NULL);
    }
    if (old_caret != info->caret)
      events[event_count++] = text_signal(path, "TextCaretMoved", "", info->caret, 0, NULL);
  }
  /* A replacement is one ordered batch: construct both halves first. A send
   * failure can still prevent delivery; invalidate instead of treating a later
   * observation as a known continuation of this potentially incomplete batch. */
  for (k = 0; k < event_count; ++k)
    if (!events[k]) events_ok = 0;
  for (k = 0; k < event_count; ++k) {
    if (events_ok && !dbus_connection_send(t->connection, events[k], NULL))
      events_ok = 0;
    if (events[k]) dbus_message_unref(events[k]);
  }
  if (!events_ok) {
    XtFree(record->text_content); record->text_content = NULL;
    record->text_known = 0;
  }
  XtFree(previous);
}
static void seed_connected_tree(Transport *t) {
  XmAccessibility *a = t->owner;
  XmAccessibleNode *node;
  XmAccessibleId *ids;
  size_t count = 0, k;
  ++a->dispatch_depth;
  for (node = a->nodes; node; node = node->next) ++count;
  if (count > UINT_MAX / sizeof(*ids)) goto done;
  ids = (XmAccessibleId *)XtMalloc((Cardinal)(count * sizeof(*ids)));
  if (!ids) goto done;
  for (node = a->nodes, k = 0; node; node = node->next) ids[k++] = node->id;
  for (k = 0; k < count && !a->destroy_requested; ++k) {
    XmAccessibleInfo info;
    TransportState *record;
    if (!XmAccessibilityQuery(a, ids[k], &info)) continue;
    if (!dbus_validate_utf8(text(info.name), NULL) ||
        !dbus_validate_utf8(text(info.value_text), NULL)) {
      XmAccessibleInfoClear(&info); continue;
    }
    record = memo_get(t, ids[k], 1);
    record->known = 1; record->states = info.states; record->role = info.role;
    XtFree(record->name); record->name = XtNewString(text(info.name));
    record->has_value = info.has_value;
    record->minimum = info.minimum_value; record->maximum = info.maximum_value;
    record->current = info.current_value; record->increment = info.minimum_increment;
    XtFree(record->value_text); record->value_text = XtNewString(text(info.value_text));
    sync_text(t, ids[k], &info, 0);
    XmAccessibleInfoClear(&info);
  }
  /* Registration exposes an already-active native window to a new client.
   * Announce this initial lifecycle once; subsequent notifications are deltas. */
  for (k = 0; k < count && !a->destroy_requested; ++k) {
    TransportState *record = memo_get(t, ids[k], 0);
    char path[96];
    if (!record) continue;
    path_for(ids[k], path, sizeof(path));
    if (record->role == XmACCESSIBLE_FRAME && (record->states & XmACCESSIBLE_STATE(1))) {
      state_event(t, path, "active", 1);
      window_event(t, ids[k], record->name, 1);
    }
  }
  for (k = 0; k < count && !a->destroy_requested; ++k) {
    TransportState *record = memo_get(t, ids[k], 0);
    char path[96];
    if (record && (record->states & XmACCESSIBLE_STATE(12))) {
      path_for(ids[k], path, sizeof(path)); state_event(t, path, "focused", 1);
    }
  }
  XtFree((char *)ids);
done:
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
}
static void embedded(DBusPendingCall *pending, void *data) {
  Transport *t = (Transport *)data;
  DBusMessage *reply = dbus_pending_call_steal_reply(pending);
  t->owner->status =
      reply && dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN
          ? XmACCESSIBILITY_CONNECTED
          : XmACCESSIBILITY_UNAVAILABLE;
  if (reply)
    dbus_message_unref(reply);
  if (t->owner->status == XmACCESSIBILITY_CONNECTED) seed_connected_tree(t);
}
static DBusObjectPathVTable vtable = {NULL, dispatch, NULL, NULL, NULL, NULL};
XmAccessibilityStatus _XmAccessibilityTransportConnect(XmAccessibility *a) {
  DBusConnection *session;
  DBusMessage *call, *reply;
  DBusError error;
  const char *address = NULL;
  Transport *t;
  int fd;
  DBusMessageIter out;
  if (a->transport) {
    if (a->status == XmACCESSIBILITY_UNAVAILABLE ||
        a->status == XmACCESSIBILITY_DISCONNECTED)
      _XmAccessibilityTransportDestroy(a);
    else
      return a->status;
  }
  if (!a->root)
    return a->status = XmACCESSIBILITY_UNAVAILABLE;
  dbus_error_init(&error);
  session = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
  if (!session) {
    dbus_error_free(&error);
    return a->status = XmACCESSIBILITY_UNAVAILABLE;
  }
  dbus_connection_set_exit_on_disconnect(session, FALSE);
  call = dbus_message_new_method_call("org.a11y.Bus", "/org/a11y/bus",
                                      "org.a11y.Bus", "GetAddress");
  reply =
      dbus_connection_send_with_reply_and_block(session, call, 1000, &error);
  dbus_message_unref(call);
  if (!reply || !dbus_message_get_args(reply, &error, DBUS_TYPE_STRING,
                                       &address, DBUS_TYPE_INVALID)) {
    if (reply)
      dbus_message_unref(reply);
    dbus_connection_close(session);
    dbus_connection_unref(session);
    dbus_error_free(&error);
    return a->status = XmACCESSIBILITY_UNAVAILABLE;
  }
  t = (Transport *)XtCalloc(1, sizeof(*t));
  if (!t) {
    dbus_message_unref(reply);
    dbus_connection_close(session);
    dbus_connection_unref(session);
    dbus_error_free(&error);
    return a->status = XmACCESSIBILITY_UNAVAILABLE;
  }
  t->owner = a;
  t->connection = dbus_connection_open_private(address, &error);
  dbus_message_unref(reply);
  dbus_connection_close(session);
  dbus_connection_unref(session);
  if (!t->connection || !dbus_bus_register(t->connection, &error)) {
    if (t->connection) {
      dbus_connection_close(t->connection);
      dbus_connection_unref(t->connection);
    }
    XtFree((char *)t);
    dbus_error_free(&error);
    return a->status = XmACCESSIBILITY_UNAVAILABLE;
  }
  dbus_error_free(&error);
  dbus_connection_set_exit_on_disconnect(t->connection, FALSE);
  if (!dbus_connection_register_object_path(t->connection, CACHEPATH, &vtable, t) ||
      !dbus_connection_register_fallback(t->connection, PREFIX, &vtable, t) ||
      !dbus_connection_get_unix_fd(t->connection, &fd)) {
    dbus_connection_close(t->connection);
    dbus_connection_unref(t->connection);
    XtFree((char *)t);
    return a->status = XmACCESSIBILITY_UNAVAILABLE;
  }
  a->transport = (XtPointer)t;
  a->status = XmACCESSIBILITY_CONNECTING;
  t->input =
      XtAppAddInput(XtWidgetToApplicationContext(a->root), fd,
                    (XtPointer)XtInputReadMask, input_ready, (XtPointer)t);
  t->timer = XtAppAddTimeOut(XtWidgetToApplicationContext(a->root), 100,
                             timer_ready, (XtPointer)t);
  call = dbus_message_new_method_call("org.a11y.atspi.Registry", ROOT,
                                      "org.a11y.atspi.Socket", "Embed");
  dbus_message_iter_init_append(call, &out);
  reference(&out, t, 1);
  if (!dbus_connection_send_with_reply(t->connection, call, &t->embed, 2000) ||
      !t->embed)
    a->status = XmACCESSIBILITY_UNAVAILABLE;
  else
    dbus_pending_call_set_notify(t->embed, embedded, t, NULL);
  dbus_message_unref(call);
  pump(t);
  return a->status;
}
void _XmAccessibilityTransportDestroy(XmAccessibility *a) {
  Transport *t = (Transport *)a->transport;
  if (!t)
    return;
  if (t->input)
    XtRemoveInput(t->input);
  if (t->timer)
    XtRemoveTimeOut(t->timer);
  if (t->embed) {
    dbus_pending_call_cancel(t->embed);
    dbus_pending_call_unref(t->embed);
  }
  dbus_connection_unregister_object_path(t->connection, CACHEPATH);
  dbus_connection_unregister_object_path(t->connection, PREFIX);
  dbus_connection_close(t->connection);
  dbus_connection_unref(t->connection);
  {
    size_t k;
    for (k = 0; k < t->memo_capacity; ++k) {
      TransportState *record = t->memo[k];
      while (record) {
        TransportState *next = record->next;
        XtFree(record->name); XtFree(record->value_text); XtFree(record->text_content); XtFree((char *)record); record = next;
      }
    }
    XtFree((char *)t->memo);
  }
  XtFree((char *)t);
  a->transport = NULL;
}
const char *_XmAccessibilityTransportBusName(XmAccessibility *a) {
  Transport *t = (Transport *)a->transport;
  return t ? text(dbus_bus_get_unique_name(t->connection)) : "";
}
static void state_event(Transport *t, const char *path, const char *name,
                          int enabled) {
  DBusMessage *message = dbus_message_new_signal(path,
      "org.a11y.atspi.Event.Object", "StateChanged");
  DBusMessageIter out, variant, array;
  dbus_int32_t value = enabled, zero = 0;
  dbus_message_iter_init_append(message, &out);
  string_value(&out, name);
  dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &value);
  dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &zero);
  dbus_message_iter_open_container(&out, DBUS_TYPE_VARIANT, "i", &variant);
  dbus_message_iter_append_basic(&variant, DBUS_TYPE_INT32, &zero);
  dbus_message_iter_close_container(&out, &variant);
  dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "{sv}", &array);
  dbus_message_iter_close_container(&out, &array);
  send_reply(t, message);
}
static void window_event(Transport *t, XmAccessibleId id, const char *name, int active) {
  DBusMessage *message;
  DBusMessageIter out, variant, array;
  dbus_int32_t zero = 0;
  char path[96];
  path_for(id, path, sizeof(path));
  message = dbus_message_new_signal(path, "org.a11y.atspi.Event.Window",
                                    active ? "Activate" : "Deactivate");
  dbus_message_iter_init_append(message, &out);
  string_value(&out, "");
  dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &zero);
  dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &zero);
  dbus_message_iter_open_container(&out, DBUS_TYPE_VARIANT, "s", &variant);
  string_value(&variant, name);
  dbus_message_iter_close_container(&out, &variant);
  dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "{sv}", &array);
  dbus_message_iter_close_container(&out, &array);
  send_reply(t, message);
}
void _XmAccessibilityTransportRemoved(XmAccessibility *a, XmAccessibleId id,
                                      XmAccessibleId parent, int index) {
  Transport *t = (Transport *)a->transport;
  DBusMessage *message;
  DBusMessageIter out;
  if (!t)
    return;
  memo_remove(t, id);
  if (a->status != XmACCESSIBILITY_CONNECTED)
    return;
  message = dbus_message_new_signal(CACHEPATH, CACHE, "RemoveAccessible");
  dbus_message_iter_init_append(message, &out);
  reference(&out, t, id);
  send_reply(t, message);
  if (parent) {
    char path[96];
    DBusMessageIter variant, array;
    dbus_int32_t position = index, zero = 0;
    path_for(parent, path, sizeof(path));
    message = dbus_message_new_signal(path, "org.a11y.atspi.Event.Object", "ChildrenChanged");
    dbus_message_iter_init_append(message, &out);
    string_value(&out, "remove");
    dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &position);
    dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &zero);
    dbus_message_iter_open_container(&out, DBUS_TYPE_VARIANT, "(so)", &variant);
    reference(&variant, t, id);
    dbus_message_iter_close_container(&out, &variant);
    dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "{sv}", &array);
    dbus_message_iter_close_container(&out, &array);
    send_reply(t, message);
  }
}
void _XmAccessibilityTransportChanged(XmAccessibility *a, XmAccessibleId id) {
  Transport *t = (Transport *)a->transport;
  DBusMessage *message;
  DBusMessageIter out, variant, array;
  XmAccessibleInfo info;
  char path[96];
  dbus_int32_t first = 0, second = 0;
  const char *detail = "accessible-name";
  TransportState *record;
  uint64_t old_states;
  int name_changed, value_changed;
  if (!t || a->status != XmACCESSIBILITY_CONNECTED ||
      !XmAccessibilityQuery(a, id, &info))
    return;
  if (!dbus_validate_utf8(text(info.name), NULL) ||
      !dbus_validate_utf8(text(info.value_text), NULL)) {
    XmAccessibleInfoClear(&info);
    return;
  }
  path_for(id, path, sizeof(path));
  record = memo_get(t, id, 1);
  old_states = record->states;
  name_changed = record->known && strcmp(text(record->name), text(info.name));
  value_changed = info.has_value && (!record->has_value ||
      record->minimum != info.minimum_value || record->maximum != info.maximum_value ||
      record->current != info.current_value || record->increment != info.minimum_increment ||
      strcmp(text(record->value_text), text(info.value_text)));
  {
    int was_active = (old_states & XmACCESSIBLE_STATE(1)) != 0;
    int active = (info.states & XmACCESSIBLE_STATE(1)) != 0;
    if (!record->known) {
      message = dbus_message_new_signal(CACHEPATH, CACHE, "AddAccessible");
      dbus_message_iter_init_append(message, &out);
      if (cache_item(&out, t, id)) {
        XmAccessibleNode *node = _XmAccessibilityNode(a, id);
        send_reply(t, message);
        if (node && node->parent) {
          char parent_path[96];
          dbus_int32_t position = _XmAccessibilityIndexInParent(a, id);
          path_for(node->parent, parent_path, sizeof(parent_path));
          message = dbus_message_new_signal(parent_path, "org.a11y.atspi.Event.Object", "ChildrenChanged");
          dbus_message_iter_init_append(message, &out);
          string_value(&out, "add");
          dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &position);
          dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &second);
          dbus_message_iter_open_container(&out, DBUS_TYPE_VARIANT, "(so)", &variant);
          reference(&variant, t, id);
          dbus_message_iter_close_container(&out, &variant);
          dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "{sv}", &array);
          dbus_message_iter_close_container(&out, &array);
          send_reply(t, message);
        }
      } else dbus_message_unref(message);
    }
    if (!_XmAccessibilityNode(a, id)) {
      XmAccessibleInfoClear(&info);
      return;
    }
    record = memo_get(t, id, 1);
    record->known = 1;
    record->states = info.states; record->role = info.role;
    record->has_value = info.has_value;
    record->minimum = info.minimum_value; record->maximum = info.maximum_value;
    record->current = info.current_value; record->increment = info.minimum_increment;
    XtFree(record->value_text); record->value_text = XtNewString(text(info.value_text));
    XtFree(record->name); record->name = XtNewString(text(info.name));
    if (was_active != active) {
      state_event(t, path, "active", active);
      if (info.role == XmACCESSIBLE_FRAME)
        window_event(t, id, info.name, active);
    }
  }
  if (name_changed) {
    message = dbus_message_new_signal(path, "org.a11y.atspi.Event.Object",
                                      "PropertyChange");
    dbus_message_iter_init_append(message, &out);
    string_value(&out, detail);
    dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &first);
    dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &second);
    dbus_message_iter_open_container(&out, DBUS_TYPE_VARIANT, "s", &variant);
    string_value(&variant, info.name);
    dbus_message_iter_close_container(&out, &variant);
    dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "{sv}", &array);
    dbus_message_iter_close_container(&out, &array);
    send_reply(t, message);
  }
  if (value_changed) {
    message = dbus_message_new_signal(path, "org.a11y.atspi.Event.Object",
                                      "PropertyChange");
    dbus_message_iter_init_append(message, &out);
    string_value(&out, "accessible-value");
    dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &first);
    dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &second);
    dbus_message_iter_open_container(&out, DBUS_TYPE_VARIANT, "d", &variant);
    dbus_message_iter_append_basic(&variant, DBUS_TYPE_DOUBLE,
                                   &info.current_value);
    dbus_message_iter_close_container(&out, &variant);
    dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "{sv}", &array);
    dbus_message_iter_close_container(&out, &array);
    send_reply(t, message);
    {
      const char *names[] = {"MinimumValue", "MaximumValue", "CurrentValue",
                             "MinimumIncrement", "Text"};
      DBusMessageIter entry;
      size_t k;
      message = dbus_message_new_signal(path, DBUS_INTERFACE_PROPERTIES,
                                        "PropertiesChanged");
      dbus_message_iter_init_append(message, &out);
      string_value(&out, VALUE);
      dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "{sv}", &array);
      for (k = 0; k < XtNumber(names); ++k) {
        dbus_message_iter_open_container(&array, DBUS_TYPE_DICT_ENTRY, NULL,
                                         &entry);
        string_value(&entry, names[k]);
        dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT,
                                         property_signature(VALUE, names[k]),
                                         &variant);
        property_value(&variant, t, _XmAccessibilityNode(a, id), &info, VALUE,
                       names[k]);
        dbus_message_iter_close_container(&entry, &variant);
        dbus_message_iter_close_container(&array, &entry);
      }
      dbus_message_iter_close_container(&out, &array);
      dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "s", &array);
      dbus_message_iter_close_container(&out, &array);
      send_reply(t, message);
      first = 0;
    }
  }
  {
    const char *names[] = {"invalid", "active", "armed", "busy", "checked",
        "collapsed", "defunct", "editable", "enabled", "expandable", "expanded",
        "focusable", "focused", "has-tooltip", "horizontal", "iconified", "modal",
        "multi-line", "multiselectable", "opaque", "pressed", "resizable",
        "selectable", "selected", "sensitive", "showing", "single-line", "stale",
        "transient", "vertical", "visible", "manages-descendants", "indeterminate",
        "required", "truncated", "animated", "invalid-entry", "supports-autocompletion",
        "selectable-text", "is-default", "visited", "checkable", "has-popup", "read-only"};
    size_t k;
    for (k = 2; k < XtNumber(names); ++k)
      if ((old_states ^ info.states) & XmACCESSIBLE_STATE(k))
        state_event(t, path, names[k], (info.states & XmACCESSIBLE_STATE(k)) != 0);
  }
  sync_text(t, id, &info, 1);
  XmAccessibleInfoClear(&info);
}

void _XmAccessibilityTransportSelectionChanged(XmAccessibility *a, XmAccessibleId id) {
  Transport *t = a ? (Transport *)a->transport : NULL;
  char path[96];
  DBusMessage *message;
  if (!t || a->destroy_requested || a->status != XmACCESSIBILITY_CONNECTED || !_XmAccessibilityNode(a, id)) return;
  path_for(id, path, sizeof(path));
  message = text_signal(path, "SelectionChanged", "", 0, 0, NULL);
  if (message) send_reply(t, message);
}
