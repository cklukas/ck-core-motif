/* Optional AT-SPI D-Bus adapter; all dispatch stays on the Xt UI thread.
 * LGPL-2.0-or-later. Protocol: at-spi2-core/xml/{Accessible,Application,Action,
 * Text,Table,Component,Socket}.xml. No ATK, GLib, GTK or Qt is linked. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "AccessibleI.h"
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <dbus/dbus.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROOT "/org/a11y/atspi/accessible/root"
#define PREFIX "/org/a11y/atspi/accessible"
#define NULLPATH "/org/a11y/atspi/null"
#define ACCESS "org.a11y.atspi.Accessible"
#define APP "org.a11y.atspi.Application"
#define ACTION "org.a11y.atspi.Action"
#define TEXT "org.a11y.atspi.Text"
#define TABLE "org.a11y.atspi.Table"
#define COMPONENT "org.a11y.atspi.Component"
typedef struct {
  XmAccessibility *owner;
  DBusConnection *connection;
  DBusPendingCall *embed;
  XtInputId input;
  XtIntervalId timer;
  int application_id;
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
  XmAccessibleNode *n;
  int count = 0;
  for (n = a->nodes; n; n = n->next)
    if (n->parent == id)
      count++;
  return count;
}
static XmAccessibleId child_at(XmAccessibility *a, XmAccessibleId id,
                               int index) {
  XmAccessibleNode *n;
  XmAccessibleId result = 0, after = 0;
  int i;
  /* Registration list is reverse-linked; expose stable creation order. */
  for (i = 0; i <= index; i++) {
    result = 0;
    for (n = a->nodes; n; n = n->next)
      if (n->parent == id && n->id > after && (!result || n->id < result))
        result = n->id;
    if (!result)
      return 0;
    after = result;
  }
  return result;
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
    if (!strcmp(name, "Name"))
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
    "<node><interface name='org.freedesktop.DBus.Introspectable'><method "
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
    "<interface name='org.a11y.atspi.Text'><method name='GetText'><arg "
    "direction='in' type='i'/><arg direction='in' type='i'/><arg "
    "direction='out' type='s'/></method><method "
    "name='GetCharacterAtOffset'><arg direction='in' type='i'/><arg "
    "direction='out' type='i'/></method></interface>"
    "<interface name='org.a11y.atspi.Table'><method "
    "name='GetAccessibleAt'><arg direction='in' type='i'/><arg direction='in' "
    "type='i'/><arg direction='out' type='(so)'/></method></interface></node>";
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
      !dbus_validate_utf8(text(info.description), NULL)) {
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
      dbus_message_iter_open_container(&out, DBUS_TYPE_VARIANT, signature,
                                       &variant);
      property_value(&variant, t, node, &info, requested, name);
      dbus_message_iter_close_container(&out, &variant);
      goto done;
    }
    if (!strcmp(method, "GetAll")) {
      const char *names[] = {"Name",
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
                             "NSelectedColumns"};
      DBusMessageIter array, entry;
      size_t i;
      if (!dbus_message_get_args(call, NULL, DBUS_TYPE_STRING, &requested,
                                 DBUS_TYPE_INVALID))
        goto invalid;
      if (!supports_interface(id, &info, requested))
        goto unsupported;
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
      int i, count = child_count(a, node->parent);
      index = -1;
      for (i = 0; i < count; i++)
        if (child_at(a, node->parent, i) == id) {
          index = i;
          break;
        }
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
      char *name =
          node->provider.action_name
              ? node->provider.action_name(node->data, (unsigned int)index)
              : XtNewString("activate");
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
          while (origin && origin->widget && !XtIsShell(origin->widget))
            origin = _XmAccessibilityNode(a, origin->parent);
        } else
          origin = _XmAccessibilityNode(a, node->parent);
        if (!origin || !XmAccessibilityQuery(a, origin->id, &parent_info))
          goto unsupported;
        if (!parent_info.has_extents) {
          XmAccessibleInfoClear(&parent_info);
          goto unsupported;
        }
        x -= parent_info.screen_x;
        y -= parent_info.screen_y;
        XmAccessibleInfoClear(&parent_info);
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
    if (!strcmp(method, "GetNSelections")) {
      index = 0;
      dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &index);
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
static void embedded(DBusPendingCall *pending, void *data) {
  Transport *t = (Transport *)data;
  DBusMessage *reply = dbus_pending_call_steal_reply(pending);
  t->owner->status =
      reply && dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN
          ? XmACCESSIBILITY_CONNECTED
          : XmACCESSIBILITY_UNAVAILABLE;
  if (reply)
    dbus_message_unref(reply);
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
  if (!dbus_connection_register_fallback(t->connection, PREFIX, &vtable, t) ||
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
  dbus_connection_unregister_object_path(t->connection, PREFIX);
  dbus_connection_close(t->connection);
  dbus_connection_unref(t->connection);
  XtFree((char *)t);
  a->transport = NULL;
}
const char *_XmAccessibilityTransportBusName(XmAccessibility *a) {
  Transport *t = (Transport *)a->transport;
  return t ? text(dbus_bus_get_unique_name(t->connection)) : "";
}
void _XmAccessibilityTransportChanged(XmAccessibility *a, XmAccessibleId id) {
  Transport *t = (Transport *)a->transport;
  DBusMessage *message;
  DBusMessageIter out, variant, array;
  XmAccessibleInfo info;
  char path[96];
  dbus_int32_t first = 0, second = 0;
  const char *detail = "accessible-name";
  if (!t || a->status != XmACCESSIBILITY_CONNECTED ||
      !XmAccessibilityQuery(a, id, &info))
    return;
  path_for(id, path, sizeof(path));
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
  message = dbus_message_new_signal(path, "org.a11y.atspi.Event.Object",
                                    "StateChanged");
  dbus_message_iter_init_append(message, &out);
  string_value(&out, "focused");
  first = (info.states & XmACCESSIBLE_STATE(12)) ? 1 : 0;
  dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &first);
  dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &second);
  dbus_message_iter_open_container(&out, DBUS_TYPE_VARIANT, "i", &variant);
  dbus_message_iter_append_basic(&variant, DBUS_TYPE_INT32, &second);
  dbus_message_iter_close_container(&out, &variant);
  dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "{sv}", &array);
  dbus_message_iter_close_container(&out, &array);
  send_reply(t, message);
  if (info.has_text) {
    const char *members[] = {"VisibleDataChanged", "TextCaretMoved"};
    size_t k;
    for (k = 0; k < 2; k++) {
      message = dbus_message_new_signal(path, "org.a11y.atspi.Event.Object",
                                        members[k]);
      dbus_message_iter_init_append(message, &out);
      string_value(&out, "");
      first = k ? info.caret : 0;
      second = 0;
      dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &first);
      dbus_message_iter_append_basic(&out, DBUS_TYPE_INT32, &second);
      dbus_message_iter_open_container(&out, DBUS_TYPE_VARIANT, "i", &variant);
      dbus_message_iter_append_basic(&variant, DBUS_TYPE_INT32, &second);
      dbus_message_iter_close_container(&out, &variant);
      dbus_message_iter_open_container(&out, DBUS_TYPE_ARRAY, "{sv}", &array);
      dbus_message_iter_close_container(&out, &array);
      send_reply(t, message);
    }
  }
  XmAccessibleInfoClear(&info);
}
