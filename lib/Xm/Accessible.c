/* Generic widget semantics and custom semantic providers. LGPL-2.0-or-later. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "AccessibleI.h"
#include "VendorSI.h"
#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xutil.h>
#include <Xm/ArrowB.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/GadgetP.h>
#include <Xm/MenuShell.h>
#include <Xm/PrimitiveP.h>
#include <Xm/RowColumnP.h>
#include <Xm/DisclosureP.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/ListP.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/ScrollBar.h>
#include <Xm/Text.h>
#include <Xm/TextP.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

static void deferred_destroy(XtPointer, XtIntervalId *);
static Boolean sync_native_list(XmAccessibility *, XmAccessibleNode *);
static void list_changed(Widget, XtPointer, XtPointer);
static XmAccessibleId focused_identity(XmAccessibility *);

/* Live-object indexes never allocate in proportion to historical IDs. */
static size_t id_hash(XmAccessibleId id, size_t capacity) {
  return ((size_t)id * UINT32_C(2654435761)) & (capacity - 1);
}
static size_t widget_hash(Widget widget, size_t capacity) {
  uintptr_t value = (uintptr_t)widget;
  value = (value >> 4) ^ (value >> 17);
  return (size_t)value & (capacity - 1);
}
static Boolean resize_index(XmAccessibility *a, size_t capacity, Boolean widgets) {
  XmAccessibleNode **buckets, *n;
  if (capacity > SIZE_MAX / sizeof(*buckets)) return False;
  buckets = (XmAccessibleNode **)calloc(capacity, sizeof(*buckets));
  if (!buckets) return False;
  for (n = a->nodes; n; n = n->next) {
    size_t index;
    if (widgets) {
      if (!n->widget) continue;
      index = widget_hash(n->widget, capacity);
      n->widget_next = buckets[index];
    } else {
      index = id_hash(n->id, capacity);
      n->id_next = buckets[index];
    }
    buckets[index] = n;
  }
  if (widgets) {
    free(a->widget_buckets); a->widget_buckets = buckets;
    a->widget_capacity = capacity;
  } else {
    free(a->id_buckets); a->id_buckets = buckets; a->id_capacity = capacity;
  }
  return True;
}
static Boolean reserve_index(XmAccessibility *a, Boolean widgets) {
  size_t capacity = widgets ? a->widget_capacity : a->id_capacity;
  size_t count = widgets ? a->widget_count : a->node_count;
  if (capacity && count < capacity) return True;
  if (capacity > SIZE_MAX / 2) return False;
  return resize_index(a, capacity ? capacity * 2 : 64, widgets);
}
static void shrink_indexes(XmAccessibility *a) {
  size_t capacity = a->id_capacity;
  while (capacity > 64 && a->node_count < capacity / 4) capacity /= 2;
  if (capacity && capacity != a->id_capacity) (void)resize_index(a, capacity, False);
  capacity = a->widget_capacity;
  while (capacity > 64 && a->widget_count < capacity / 4) capacity /= 2;
  if (capacity && capacity != a->widget_capacity) (void)resize_index(a, capacity, True);
}
XmAccessibleNode *_XmAccessibilityNode(XmAccessibility *a, XmAccessibleId id) {
  XmAccessibleNode *n;
  if (!a || !id || !a->id_capacity) return NULL;
  for (n = a->id_buckets[id_hash(id, a->id_capacity)]; n; n = n->id_next)
    if (n->id == id) return n;
  return NULL;
}
size_t _XmAccessibilityChildCount(XmAccessibility *a, XmAccessibleId id) {
  XmAccessibleNode *n = _XmAccessibilityNode(a, id);
  if (n && n->widget && XmIsList(n->widget)) (void)sync_native_list(a, n);
  return n ? n->child_count : 0;
}
XmAccessibleId _XmAccessibilityChildAt(XmAccessibility *a, XmAccessibleId id, size_t index) {
  XmAccessibleNode *n = _XmAccessibilityNode(a, id);
  if (n && n->widget && XmIsList(n->widget)) (void)sync_native_list(a, n);
  return n && index < n->child_count ? n->children[index]->id : 0;
}
int _XmAccessibilityIndexInParent(XmAccessibility *a, XmAccessibleId id) {
  XmAccessibleNode *n = _XmAccessibilityNode(a, id);
  return n && n->parent ? (int)n->child_index : -1;
}
static Boolean bind_widget(XmAccessibility *a, XmAccessibleNode *n, Widget widget) {
  size_t index;
  if (!reserve_index(a, True)) return False;
  index = widget_hash(widget, a->widget_capacity);
  n->widget = widget; n->widget_next = a->widget_buckets[index];
  a->widget_buckets[index] = n; ++a->widget_count;
  return True;
}
static void unbind_widget(XmAccessibility *a, XmAccessibleNode *n) {
  XmAccessibleNode **link;
  if (!n->widget) return;
  link = &a->widget_buckets[widget_hash(n->widget, a->widget_capacity)];
  while (*link && *link != n) link = &(*link)->widget_next;
  if (*link) { *link = n->widget_next; --a->widget_count; }
  n->widget_next = NULL;
}
void _XmAccessibilityDispatchDone(XmAccessibility *a) {
  if (!a || a->dispatch_depth) return;
  while (a->retired_nodes) {
    XmAccessibleNode *n = a->retired_nodes;
    a->retired_nodes = n->retired_next;
    free(n->list_selected); free(n);
  }
  if (a->destroy_requested && !a->deferred_destroy)
    a->deferred_destroy = XtAppAddTimeOut(a->app, 0, deferred_destroy, (XtPointer)a);
}
static Widget shell_for(Widget widget) {
  while (widget && !XtIsShell(widget))
    widget = XtParent(widget);
  return widget;
}
/* MenuShell has its own logical focus record. Keep its leaf focus distinct
 * from the owning WM frame, whose activation the override shell borrows. */
static Widget input_shell(XmAccessibility *a) {
  Window focus;
  int revert;
  Display *display;
  if (!a->root) return NULL;
  display = XtDisplayOfObject(a->root);
  XGetInputFocus(display, &focus, &revert);
  if (focus == None || focus == PointerRoot) return NULL;
  return shell_for(XtWindowToWidget(display, focus));
}
static Widget active_shell(XmAccessibility *a) {
  Widget shell = input_shell(a);
  while (shell && !XtIsWMShell(shell)) shell = shell_for(XtParent(shell));
  return shell;
}
static Widget focused_widget(XmAccessibility *a) {
  Widget shell = input_shell(a);
  return shell ? XmGetFocusWidget(shell) : NULL;
}
static Boolean menu_pane(Widget w) {
  unsigned char type;
  if (!w || !XmIsRowColumn(w)) return False;
  XtVaGetValues(w, XmNrowColumnType, &type, NULL);
  return type == XmMENU_PULLDOWN || type == XmMENU_POPUP || type == XmMENU_BAR;
}
static Boolean submenu_posted(Widget cascade, Widget submenu) {
  Widget parent;
  XWindowAttributes attributes;
  if (!submenu || !XtIsRealized(submenu) || !XmIsRowColumn(submenu) ||
      RC_CascadeBtn(submenu) != cascade) return False;
  parent = XtParent(submenu);
  if (!parent || !XmIsMenuShell(parent)) return False;
  return XGetWindowAttributes(XtDisplay(submenu), XtWindow(submenu), &attributes) &&
         attributes.map_state == IsViewable;
}
/* Mapping a MenuShell does not remap its already-managed menu children. Notify
 * the actual posting cascade as well so expanded follows native posting. */
static void menu_changed(XmAccessibility *a, Widget w) {
  WidgetList children;
  Cardinal count, k;
  if (menu_pane(w)) {
    Widget cascade = RC_CascadeBtn(w);
    if (cascade) XmAccessibilityChanged(a, XmAccessibilityWidget(a, cascade));
  } else if (XmIsMenuShell(w)) {
    XtVaGetValues(w, XtNchildren, &children, XtNnumChildren, &count, NULL);
    /* Snapshot IDs: notification can run client/provider code and retire nodes. */
    XmAccessibleId *ids = (XmAccessibleId *)XtMalloc(count * sizeof(*ids));
    for (k = 0; k < count; ++k)
      ids[k] = menu_pane(children[k])
                   ? XmAccessibilityWidget(a, RC_CascadeBtn(children[k])) : 0;
    for (k = 0; k < count; ++k) if (ids[k]) XmAccessibilityChanged(a, ids[k]);
    XtFree((char *)ids);
  }
}
static void node_destroyed(Widget w, XtPointer data, XtPointer call) {
  XmAccessibility *a = (XmAccessibility *)data;
  XmAccessibleNode *n;
  (void)call;
  ++a->dispatch_depth;
  if (w == a->root) {
    a->root = NULL;
    for (n = a->nodes; n; n = n->next)
      if (n->parent == 1 && n->widget && n->widget != w) {
        a->root = n->widget;
        break;
      }
  }
  {
    XmAccessibleId id = XmAccessibilityWidget(a, w);
    n = _XmAccessibilityNode(a, id);
    if (n) {
      unbind_widget(a, n);
      n->widget = NULL;
      XmAccessibilityRemove(a, id);
      XmAccessibilityChanged(a, 1);
    }
  }
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
}
static void notify_focus(XmAccessibility *a) {
  Widget shell;
  XmAccessibleId active, next;
  if (a->destroy_requested) return;
  shell = active_shell(a);
  active = shell ? XmAccessibilityWidget(a, shell) : 0;
  next = focused_identity(a);
  if (active != a->active_shell_node) {
    XmAccessibleId previous = a->active_shell_node;
    a->active_shell_node = active;
    if (previous) XmAccessibilityChanged(a, previous);
    if (a->destroy_requested) return;
    if (active) XmAccessibilityChanged(a, active);
  }
  if (a->destroy_requested) return;
  if (next != a->focused_node) {
    XmAccessibleId previous = a->focused_node;
    a->focused_node = next;
    if (previous)
      XmAccessibilityChanged(a, previous);
    if (a->destroy_requested) return;
    if (next)
      XmAccessibilityChanged(a, next);
  }
}
/* Ancestor geometry can change descendants' SHOWING without generating an
 * event on those descendants. Snapshot live native IDs before notifying: a
 * provider/transport callback may retire widgets or the bridge reentrantly.
 * Custom provider geometry remains explicitly owned by that provider.
 */
static void native_descendants_changed(XmAccessibility *a, XmAccessibleId id) {
  XmAccessibleNode *ancestor = _XmAccessibilityNode(a, id), *node;
  XmAccessibleId *ids;
  size_t count = 0, k;
  Widget root;
  if (!ancestor || !(root = ancestor->widget) || a->destroy_requested ||
      a->node_count > SIZE_MAX / sizeof(*ids)) return;
  ids = calloc(a->node_count ? a->node_count : 1, sizeof(*ids));
  if (!ids) return;
  for (node = a->nodes; node; node = node->next) {
    Widget current = node->list_owner ? node->list_owner : node->widget;
    if (!current || node->id == id) continue;
    for (; current; current = XtParent(current)) {
      if (current == root) { ids[count++] = node->id; break; }
      if (XtIsShell(current)) break;
    }
  }
  for (k = 0; k < count && !a->destroy_requested; ++k)
    if (_XmAccessibilityNode(a, ids[k])) XmAccessibilityChanged(a, ids[k]);
  free(ids);
}
static void widget_event(Widget w, XtPointer data, XEvent *event,
                         Boolean *cont) {
  XmAccessibility *a = (XmAccessibility *)data;
  (void)event;
  (void)cont;
  ++a->dispatch_depth;
  notify_focus(a);
  if (event->type == MapNotify || event->type == UnmapNotify) menu_changed(a, w);
  if (XmIsList(w)) list_changed(w, a, NULL);
  {
    XmAccessibleId id = XmAccessibilityWidget(a, w);
    XmAccessibilityChanged(a, id);
    if (event->type == MapNotify || event->type == UnmapNotify ||
        event->type == ConfigureNotify)
      native_descendants_changed(a, id);
  }
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
}
/* Raw handlers preserve native event selection. Xt invokes these before its
 * translation manager; consumption stops both later handlers and actions. The
 * independent key lookup must not advance the widget's XIM/compose context. */
static void native_key_event(Widget w, XtPointer data, XEvent *event,
                             Boolean *cont) {
  XmAccessibility *a = (XmAccessibility *)data;
  XKeyEvent key;
  KeySym symbol = NoSymbol;
  char local[64], encoded[5];
  const char *label;
  uint32_t scalar = 0;
  Boolean is_text = False, consumed;
  if (event->type != KeyPress && event->type != KeyRelease) return;
  if (a->destroy_requested || a->status != XmACCESSIBILITY_CONNECTED) return;
  ++a->dispatch_depth;
  key = event->xkey;
  (void)XLookupString(&key, local, sizeof(local), &symbol, NULL);
  if ((symbol >= 0x20 && symbol <= 0x7e) || (symbol >= 0xa0 && symbol <= 0xff))
    scalar = (uint32_t)symbol;
  else if ((symbol & 0xff000000UL) == 0x01000000UL)
    scalar = (uint32_t)(symbol & 0x00ffffffUL);
  if (scalar && scalar <= 0x10ffff && !(scalar >= 0xd800 && scalar <= 0xdfff) &&
      iswprint((wint_t)scalar)) {
    if (scalar < 0x80) { encoded[0] = (char)scalar; encoded[1] = 0; }
    else if (scalar < 0x800) {
      encoded[0] = (char)(0xc0 | (scalar >> 6));
      encoded[1] = (char)(0x80 | (scalar & 63)); encoded[2] = 0;
    } else if (scalar < 0x10000) {
      encoded[0] = (char)(0xe0 | (scalar >> 12));
      encoded[1] = (char)(0x80 | ((scalar >> 6) & 63));
      encoded[2] = (char)(0x80 | (scalar & 63)); encoded[3] = 0;
    } else {
      encoded[0] = (char)(0xf0 | (scalar >> 18));
      encoded[1] = (char)(0x80 | ((scalar >> 12) & 63));
      encoded[2] = (char)(0x80 | ((scalar >> 6) & 63));
      encoded[3] = (char)(0x80 | (scalar & 63)); encoded[4] = 0;
    }
    label = encoded; is_text = True;
  } else {
    /* Legacy keysyms have stable X11 names. Do not mistake an XIM commit or a
     * guessed character for the physical event's string. */
    label = XKeysymToString(symbol);
    if (!label) label = "";
  }
  consumed = _XmAccessibilityTransportKey(a, &key, symbol, label, is_text);
  if (consumed || w->core.being_destroyed) *cont = False;
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
}
static void text_value_changed(Widget w, XtPointer data, XtPointer call) {
  (void)call;
  XmAccessibilityChanged((XmAccessibility *)data,
                         XmAccessibilityWidget((XmAccessibility *)data, w));
}
static const char *scroll_callbacks[] = {
    XmNvalueChangedCallback, XmNdragCallback,          XmNincrementCallback,
    XmNdecrementCallback,    XmNpageIncrementCallback, XmNpageDecrementCallback,
    XmNtoTopCallback,        XmNtoBottomCallback};
static void resource_changed(Widget hooks, XtPointer data, XtPointer call) {
  XmAccessibility *a = (XmAccessibility *)data;
  XtChangeHookData change = (XtChangeHookData)call;
  XmAccessibleId id;
  (void)hooks;
  if (!strcmp(change->type, XtHsetValues) && (XmIsScrollBar(change->widget) || XmIsList(change->widget)) &&
      (id = XmAccessibilityWidget(a, change->widget)))
    { if (XmIsList(change->widget)) list_changed(change->widget, a, NULL);
      else XmAccessibilityChanged(a, id); }
}
static void watch_display(XmAccessibility *a, Widget widget) {
  XmAccessibleDisplayHook *hook;
  Widget hooks = XtHooksOfDisplay(XtDisplayOfObject(widget));
  for (hook = a->display_hooks; hook; hook = hook->next)
    if (hook->hooks == hooks)
      return;
  hook = (XmAccessibleDisplayHook *)XtMalloc(sizeof(*hook));
  hook->hooks = hooks;
  hook->next = a->display_hooks;
  a->display_hooks = hook;
  XtAddCallback(hooks, XtNchangeHook, resource_changed, (XtPointer)a);
}
static void disclosure_changed(Widget w, XtPointer data, XtPointer call) {
  XmAccessibility *a = (XmAccessibility *)data;
  XmAccessibleId parent = XmAccessibilityWidget(a, w);
  XmAccessibleNode *n;
  (void)call;
  ++a->dispatch_depth;
  XmAccessibilityChanged(a, parent);
  for (n = a->nodes; n; n = n->next)
    if (n->parent == parent && n->widget != XmDisclosureGetContent(w))
      XmAccessibilityChanged(a, n->id);
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
}
static void children_changed(XmAccessibleNode *node) {
  if (node->children_serial != UINT64_MAX) ++node->children_serial;
}
static XmAccessibleNode *add_node(XmAccessibility *a, XmAccessibleId parent) {
  XmAccessibleNode *n, *p = parent ? _XmAccessibilityNode(a, parent) : NULL;
  size_t index;
  if (a->next_id == UINT32_MAX || (parent && (!p || p->child_count >= INT_MAX)) ||
      !reserve_index(a, False)) return NULL;
  if (p && p->child_count == p->child_capacity) {
    size_t capacity = p->child_capacity ? p->child_capacity * 2 : 4;
    XmAccessibleNode **children;
    if (capacity < p->child_capacity || capacity > SIZE_MAX / sizeof(*children)) return NULL;
    children = (XmAccessibleNode **)realloc(p->children, capacity * sizeof(*children));
    if (!children) return NULL;
    p->children = children; p->child_capacity = capacity;
  }
  n = (XmAccessibleNode *)calloc(1, sizeof(*n));
  if (!n) return NULL;
  n->id = ++a->next_id; n->parent = parent;
  if (p) { n->child_index = p->child_count; p->children[p->child_count++] = n; children_changed(p); }
  n->next = a->nodes;
  if (n->next) n->next->previous = n;
  a->nodes = n;
  index = id_hash(n->id, a->id_capacity);
  n->id_next = a->id_buckets[index]; a->id_buckets[index] = n; ++a->node_count;
  return n;
}
XmAccessibleId XmAccessibilityWidget(XmAccessibility *a, Widget w) {
  XmAccessibleNode *n;
  if (!a || !w || !a->widget_capacity) return 0;
  for (n = a->widget_buckets[widget_hash(w, a->widget_capacity)]; n; n = n->widget_next)
    if (n->widget == w) return n->id;
  return 0;
}
static int native_item_position(XmAccessibility *a, XmAccessibleNode *n) {
  XmAccessibleNode *parent = n ? _XmAccessibilityNode(a, n->parent) : NULL;
  XmListWidget list;
  int k;
  if (!n || !n->list_owner || !parent || parent->widget != n->list_owner ||
      !XmIsList(parent->widget) || parent->widget->core.being_destroyed) return -1;
  list = (XmListWidget)parent->widget;
  k = n->list_position;
  if (k >= 0 && k < list->list.itemCount && list->list.InternalList[k]->accessible_identity == n->list_token) return k;
  for (k = 0; k < list->list.itemCount; ++k)
    if (list->list.InternalList[k]->accessible_identity == n->list_token) return k;
  return -1;
}
static Boolean sync_native_list(XmAccessibility *a, XmAccessibleNode *parent) {
  XmListWidget list = (XmListWidget)parent->widget;
  XmAccessibleNode **old = NULL, **next = NULL, **buckets = NULL;
  size_t old_count = parent->child_count, count, capacity = 4, k, j, custom = 0;
  XmAccessibleId first_new = a->next_id;
  Boolean complete = False;
  XmAccessibleId *selected = NULL;
  size_t selected_count = 0;
  Boolean selection_changed = False;
  if (parent->list_sync_busy) return parent->list_sync_valid;
  if (!parent->widget || !XmIsList(parent->widget) || parent->widget->core.being_destroyed) return False;
  if (parent->list_sync_valid && list->list.accessibility_revision != UINT64_MAX &&
      parent->list_revision == list->list.accessibility_revision) return True;
  parent->list_sync_busy = True;
  count = (size_t)list->list.itemCount;
  for (k = 0; k < old_count; ++k) if (!parent->children[k]->list_owner) ++custom;
  if (count > INT_MAX || count > SIZE_MAX / sizeof(*next) - custom) goto done;
  while (capacity < old_count * 2) {
    if (capacity > SIZE_MAX / 2) goto done;
    capacity *= 2;
  }
  old = calloc(old_count ? old_count : 1, sizeof(*old));
  next = calloc(count + custom ? count + custom : 1, sizeof(*next));
  buckets = calloc(capacity, sizeof(*buckets));
  selected = calloc(count ? count : 1, sizeof(*selected));
  if (!old || !next || !buckets || !selected) goto done;
  if (old_count) memcpy(old, parent->children, old_count * sizeof(*old));
  for (k = 0; k < old_count; ++k) if (old[k]->list_owner) {
    j = (size_t)(old[k]->list_token ^ (old[k]->list_token >> 32)) & (capacity - 1);
    old[k]->list_hash_next = buckets[j]; buckets[j] = old[k];
  }
  for (k = 0; k < count; ++k) {
    uint64_t token = list->list.InternalList[k]->accessible_identity;
    XmAccessibleNode *n;
    if (!token) goto done;
    j = (size_t)(token ^ (token >> 32)) & (capacity - 1);
    for (n = buckets[j]; n && n->list_token != token; n = n->list_hash_next) {}
    if (!n) {
      n = add_node(a, parent->id);
      if (!n) goto done;
      n->list_owner = (Widget)list; n->list_token = token;
    }
    n->list_position = (int)k;
    next[k] = n;
    if (list->list.InternalList[k]->selected)
      selected[selected_count++] = n->id;
  }
  j = count;
  for (k = 0; k < old_count; ++k) if (!old[k]->list_owner) next[j++] = old[k];
  /* Remove retired tokens before publishing new children in their real order. */
  for (k = 0; k < old_count; ++k)
    if (old[k]->list_owner && native_item_position(a, old[k]) < 0)
      XmAccessibilityRemove(a, old[k]->id);
  if (count + custom) memcpy(parent->children, next, (count + custom) * sizeof(*next));
  parent->child_count = count + custom;
  children_changed(parent);
  for (k = 0; k < parent->child_count; ++k) parent->children[k]->child_index = k;
  selection_changed = parent->list_selected_count != selected_count ||
      (selected_count && memcmp(parent->list_selected, selected, selected_count * sizeof(*selected)));
  free(parent->list_selected); parent->list_selected = selected;
  parent->list_selected_count = selected_count; selected = NULL;
  complete = True;
done:
  if (!complete) {
    /* Roll back unpublished admissions. Old IDs remain fail-closed because all
     * queries/actions validate their token against the current native model. */
    for (k = parent->child_count; k > 0; --k)
      if (parent->children[k-1]->id > first_new) XmAccessibilityRemove(a, parent->children[k-1]->id);
  }
  free(selected); free(buckets); free(old); free(next);
  parent->list_sync_valid = complete;
  if (complete) parent->list_revision = list->list.accessibility_revision;
  parent->list_sync_busy = False;
  if (complete) for (k = 0; k < parent->child_count && !a->destroy_requested; ++k)
    if (parent->children[k]->id > first_new) XmAccessibilityChanged(a, parent->children[k]->id);
  if (complete && selection_changed) {
    _XmAccessibilityTransportSelectionChanged(a, parent->id);
  }
  return complete;
}
static XmAccessibleId focused_identity(XmAccessibility *a) {
  Widget w = focused_widget(a);
  XmAccessibleNode *parent = _XmAccessibilityNode(a, XmAccessibilityWidget(a, w));
  if (parent && XmIsList(w) && sync_native_list(a, parent)) {
    int position = ((XmListWidget)w)->list.CurrentKbdItem;
    if (position >= 0 && (size_t)position < parent->child_count && parent->children[position]->list_owner)
      return parent->children[position]->id;
  }
  return parent ? parent->id : 0;
}
static void list_changed(Widget w, XtPointer data, XtPointer call) {
  XmAccessibility *a = data;
  XmAccessibleNode *parent;
  XmAccessibleId *ids;
  size_t k, count;
  (void)call;
  ++a->dispatch_depth;
  parent = _XmAccessibilityNode(a, XmAccessibilityWidget(a, w));
  if (!parent || !sync_native_list(a, parent)) goto done;
  count = parent->child_count;
  ids = calloc(count ? count : 1, sizeof(*ids));
  if (!ids) goto done;
  for (k = 0; k < count; ++k) ids[k] = parent->children[k]->id;
  XmAccessibilityChanged(a, parent->id);
  for (k = 0; k < count && !a->destroy_requested; ++k) XmAccessibilityChanged(a, ids[k]);
  free(ids);
  notify_focus(a);
done:
  --a->dispatch_depth; _XmAccessibilityDispatchDone(a);
}
static void add_widgets(XmAccessibility *a, Widget w, XmAccessibleId parent) {
  XmAccessibleId id = XmAccessibilityWidget(a, w);
  WidgetList children = NULL;
  Cardinal count = 0, i;
  XmAccessibleNode *n;
  if (!id) {
    watch_display(a, w);
    n = add_node(a, parent);
    if (!n)
      return;
    if (!bind_widget(a, n, w)) { XmAccessibilityRemove(a, n->id); return; }
    if (XmIsScrollBar(w)) {
      size_t k;
      for (k = 0; k < XtNumber(scroll_callbacks); ++k)
        XtAddCallback(w, scroll_callbacks[k], text_value_changed, (XtPointer)a);
    }
    id = n->id;
    if (XmIsList(w)) XtAddCallback(w, XmNaccessibilityChangedCallback, list_changed, a);
    XtAddCallback(w, XmNdestroyCallback, node_destroyed, (XtPointer)a);
    if (XmIsDisclosure(w))
      XtAddCallback(w, XmNdisclosureStateCallback, disclosure_changed,
                    (XtPointer)a);
    if (XmIsText(w) || XmIsTextField(w))
      XtAddCallback(w, XmNvalueChangedCallback, text_value_changed,
                    (XtPointer)a);
    if (XtIsWidget(w)) {
      XtAddEventHandler(w,
                        FocusChangeMask | StructureNotifyMask |
                            PropertyChangeMask | KeyReleaseMask |
                            ButtonReleaseMask,
                        False, widget_event, (XtPointer)a);
      XtInsertRawEventHandler(w, KeyPressMask | KeyReleaseMask, False,
                              native_key_event, (XtPointer)a, XtListHead);
    }
  }
  if (XtIsWidget(w)) {
    CoreWidget core = (CoreWidget)w;
    for (i = 0; i < core->core.num_popups; i++)
      add_widgets(a, core->core.popup_list[i], id);
  }
  if (XtIsComposite(w)) {
    XtVaGetValues(w, XtNchildren, &children, XtNnumChildren, &count, NULL);
    for (i = 0; i < count; i++)
      add_widgets(a, children[i], id);
  }
  if (XmIsList(w)) (void)sync_native_list(a, _XmAccessibilityNode(a, id));
}
XmAccessibility *XmAccessibilityCreate(Widget root, const char *name) {
  XmAccessibility *a = (XmAccessibility *)XtCalloc(1, sizeof(*a));
  XmAccessibleNode *n;
  a->root = root;
  a->app = XtWidgetToApplicationContext(root);
  a->name = XtNewString(name ? name : XtName(root));
  a->status = XmACCESSIBILITY_NOT_BUILT;
  n = add_node(a, 0);
  if (!n) { XmAccessibilityDestroy(a); return NULL; }
  add_widgets(a, root, 1);
  return a;
}
Boolean XmAccessibilityAttachShell(XmAccessibility *a, Widget shell) {
  if (!a || !shell || !XtIsShell(shell) ||
      XtWidgetToApplicationContext(shell) != a->app)
    return False;
  if (!a->root)
    a->root = shell;
  add_widgets(a, shell, 1);
  XmAccessibilityChanged(a, 1);
  return True;
}
void XmAccessibilityRefresh(XmAccessibility *a) {
  XmAccessibleNode *root;
  size_t i;
  if (!a || !(root = _XmAccessibilityNode(a, 1))) return;
  ++a->dispatch_depth;
  for (i = 0; i < root->child_count; ++i) {
    XmAccessibleNode *n = root->children[i];
    if (n->widget) add_widgets(a, n->widget, 1);
  }
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
}
XmAccessibleId XmAccessibilityRoot(XmAccessibility *a) { return a ? 1 : 0; }
static XmAccessibleId add_impl(XmAccessibility *a, XmAccessibleId parent,
                                  const XmAccessibleProvider *provider,
                                  XtPointer data) {
  XmAccessibleNode *n;
  if (!a || !provider || !provider->query || !_XmAccessibilityNode(a, parent))
    return 0;
  n = add_node(a, parent);
  if (!n)
    return 0;
  n->provider = *provider;
  n->data = data;
  {
    XmAccessibleId id = n->id;
    XmAccessibilityChanged(a, parent);
    if (_XmAccessibilityNode(a, id)) XmAccessibilityChanged(a, id);
    return _XmAccessibilityNode(a, id) ? id : 0;
  }
}
static void remove_callbacks(XmAccessibility *a, XmAccessibleNode *n) {
  if (n->widget) {
    if (XmIsList(n->widget)) XtRemoveCallback(n->widget, XmNaccessibilityChangedCallback, list_changed, a);
    if (XmIsScrollBar(n->widget)) {
      size_t k;
      for (k = 0; k < XtNumber(scroll_callbacks); ++k)
        XtRemoveCallback(n->widget, scroll_callbacks[k], text_value_changed,
                         (XtPointer)a);
    }
    XtRemoveCallback(n->widget, XmNdestroyCallback, node_destroyed,
                     (XtPointer)a);
    if (XmIsDisclosure(n->widget))
      XtRemoveCallback(n->widget, XmNdisclosureStateCallback,
                       disclosure_changed, (XtPointer)a);
    if (XmIsText(n->widget) || XmIsTextField(n->widget))
      XtRemoveCallback(n->widget, XmNvalueChangedCallback, text_value_changed,
                       (XtPointer)a);
    if (XtIsWidget(n->widget)) {
      XtRemoveRawEventHandler(n->widget, KeyPressMask | KeyReleaseMask, False,
                              native_key_event, (XtPointer)a);
      XtRemoveEventHandler(n->widget,
                           FocusChangeMask | StructureNotifyMask |
                               PropertyChangeMask | KeyReleaseMask |
                               ButtonReleaseMask,
                           False, widget_event, (XtPointer)a);
    }
  }
}
void XmAccessibilityRemove(XmAccessibility *a, XmAccessibleId id) {
  XmAccessibleNode *root = _XmAccessibilityNode(a, id), *n, *parent;
  size_t index;
  if (!root || id <= 1) return;
  parent = _XmAccessibilityNode(a, root->parent);
  /* Only the surviving parent's array is shifted. Descendants are popped
     iteratively, avoiding both quadratic array shifts and recursive stacks. */
  if (parent) {
    index = root->child_index;
    --parent->child_count;
    children_changed(parent);
    memmove(parent->children + index, parent->children + index + 1,
            (parent->child_count - index) * sizeof(*parent->children));
    for (; index < parent->child_count; ++index) parent->children[index]->child_index = index;
    if (!parent->child_count) {
      free(parent->children); parent->children = NULL; parent->child_capacity = 0;
    } else if (parent->child_capacity > 4 && parent->child_count < parent->child_capacity / 4) {
      size_t capacity = parent->child_capacity / 2;
      XmAccessibleNode **children = (XmAccessibleNode **)realloc(parent->children, capacity * sizeof(*children));
      if (children) { parent->children = children; parent->child_capacity = capacity; }
    }
  }
  n = root;
  while (n) {
    XmAccessibleNode **link;
    if (n->child_count) { n = n->children[--n->child_count]; continue; }
    parent = n == root ? NULL : _XmAccessibilityNode(a, n->parent);
    remove_callbacks(a, n);
    unbind_widget(a, n);
    link = &a->id_buckets[id_hash(n->id, a->id_capacity)];
    while (*link && *link != n) link = &(*link)->id_next;
    if (*link) *link = n->id_next;
    if (n->previous) n->previous->next = n->next; else a->nodes = n->next;
    if (n->next) n->next->previous = n->previous;
    --a->node_count;
    n->removed = True;
    memset(&n->provider, 0, sizeof(n->provider)); n->data = NULL; n->widget = NULL;
    free(n->children); n->children = NULL; n->child_capacity = 0;
    _XmAccessibilityTransportRemoved(a, n->id, n->parent, (int)n->child_index);
    if (a->dispatch_depth) { n->retired_next = a->retired_nodes; a->retired_nodes = n; }
    else { free(n->list_selected); free(n); }
    n = parent;
  }
  shrink_indexes(a);
}
static void deferred_destroy(XtPointer data, XtIntervalId *id) {
  XmAccessibility *a = (XmAccessibility *)data;
  (void)id;
  a->deferred_destroy = 0;
  if (a->dispatch_depth) { a->destroy_requested = True; return; }
  XmAccessibilityDestroy(a);
}
void XmAccessibilityDestroy(XmAccessibility *a) {
  XmAccessibleNode *n;
  if (!a)
    return;
  if (a->dispatch_depth) { a->destroy_requested = True; return; }
  a->destroy_requested = False;
  if (a->deferred_destroy)
    XtRemoveTimeOut(a->deferred_destroy);
  while (a->display_hooks) {
    XmAccessibleDisplayHook *hook = a->display_hooks;
    a->display_hooks = hook->next;
    XtRemoveCallback(hook->hooks, XtNchangeHook, resource_changed,
                     (XtPointer)a);
    XtFree((char *)hook);
  }
  _XmAccessibilityTransportDestroy(a);
  while (a->nodes) {
    n = a->nodes;
    if (n->id == 1) {
      a->nodes = n->next;
      free(n->children); free(n->list_selected); free(n);
    } else
      XmAccessibilityRemove(a, n->id);
  }
  _XmAccessibilityDispatchDone(a);
  free(a->id_buckets); free(a->widget_buckets);
  XtFree(a->name);
  XtFree((char *)a);
}
void XmAccessibleInfoClear(XmAccessibleInfo *i) {
  if (!i)
    return;
  XtFree(i->name);
  XtFree(i->description);
  XtFree(i->value_text);
  memset(i, 0, sizeof(*i));
}
/* XmText strings use the active locale. Convert Unicode scalar values to UTF-8
   explicitly instead of sending locale bytes as D-Bus UTF-8 strings. */
static char *locale_utf8(const char *s) {
  mbstate_t state;
  const char *p = s;
  size_t count;
  wchar_t *wide;
  char *result, *out;
  size_t i;
  memset(&state, 0, sizeof(state));
  count = mbsrtowcs(NULL, &p, 0, &state);
  if (count == (size_t)-1 || count > (SIZE_MAX - 1) / 4)
    return NULL;
  wide = (wchar_t *)XtMalloc((count + 1) * sizeof(*wide));
  p = s;
  memset(&state, 0, sizeof(state));
  if (mbsrtowcs(wide, &p, count + 1, &state) == (size_t)-1) {
    XtFree((char *)wide);
    return NULL;
  }
  result = (char *)XtMalloc(count * 4 + 1);
  out = result;
  for (i = 0; i < count; i++) {
    uint32_t c = (uint32_t)wide[i];
    if (c > 0x10ffff || (c >= 0xd800 && c <= 0xdfff)) {
      XtFree(result);
      XtFree((char *)wide);
      return NULL;
    }
    if (c < 0x80)
      *out++ = (char)c;
    else if (c < 0x800) {
      *out++ = (char)(0xc0 | (c >> 6));
      *out++ = (char)(0x80 | (c & 63));
    } else if (c < 0x10000) {
      *out++ = (char)(0xe0 | (c >> 12));
      *out++ = (char)(0x80 | ((c >> 6) & 63));
      *out++ = (char)(0x80 | (c & 63));
    } else {
      *out++ = (char)(0xf0 | (c >> 18));
      *out++ = (char)(0x80 | ((c >> 12) & 63));
      *out++ = (char)(0x80 | ((c >> 6) & 63));
      *out++ = (char)(0x80 | (c & 63));
    }
  }
  *out = 0;
  XtFree((char *)wide);
  return result;
}
/* Intersect one native region with every physical ancestor's content clip.
 * IsViewable alone includes mapped children outside a scrolled viewport.
 * Keep the intersection while translating to each parent: testing the original
 * region against each ancestor separately can accept an empty common clip.
 * A popup shell has its own window ancestry, independent of its logical owner.
 * Use actual window geometry: shell-child Xt resource coordinates can carry
 * top-level placement rather than the child's position in its native parent.
 */
static Boolean native_region_showing(Widget w, int64_t left, int64_t top,
                                     int64_t right, int64_t bottom) {
  Widget current;
  if (!XtIsRealized(w)) return False;
  for (current = w; current; current = XtParent(current)) {
    int x, y, width, height, border;
    if (!XtIsShell(current) && !XtIsManaged(current)) return False;
    if (XtIsWidget(current)) {
      XWindowAttributes attributes;
      if (!XGetWindowAttributes(XtDisplay(current), XtWindow(current), &attributes) ||
          attributes.map_state != IsViewable) return False;
      x = attributes.x; y = attributes.y;
      width = attributes.width; height = attributes.height;
      border = attributes.border_width;
    } else {
      Position local_x, local_y;
      Dimension local_width, local_height;
      XtVaGetValues(current, XmNx, &local_x, XmNy, &local_y,
                    XmNwidth, &local_width, XmNheight, &local_height, NULL);
      x = local_x; y = local_y; width = local_width; height = local_height;
      border = 0; /* Gadgets paint in their parent's native drawable. */
    }
    if (left < 0) left = 0;
    if (top < 0) top = 0;
    if (right > width) right = width;
    if (bottom > height) bottom = height;
    if (left >= right || top >= bottom) return False;
    if (XtIsShell(current)) return True;
    left += (int64_t)x + border; right += (int64_t)x + border;
    top += (int64_t)y + border; bottom += (int64_t)y + border;
  }
  return False;
}
static Boolean native_item_query(XmAccessibility *a, XmAccessibleNode *n, XmAccessibleInfo *info) {
  int position = native_item_position(a, n);
  XmListWidget list;
  XmString value;
  char *local;
  Position x, y, sx, sy;
  Dimension width, height;
  if (position < 0 || !_XmAccessibilityNode(a, n->parent)->list_sync_valid) return False;
  list = (XmListWidget)n->list_owner;
  value = list->list.items[position];
  local = XmStringUnparse(value, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
  info->name = locale_utf8(local ? local : ""); XtFree(local);
  if (!info->name) return False;
  info->role = XmACCESSIBLE_LIST_ITEM;
  info->action_count = 1;
  info->states = XmACCESSIBLE_STATE(22) | XmACCESSIBLE_STATE(30);
  if (XtIsSensitive((Widget)list)) info->states |= XmACCESSIBLE_STATE(8) | XmACCESSIBLE_STATE(24);
  if (list->primitive.traversal_on) info->states |= XmACCESSIBLE_STATE(11);
  if (list->list.InternalList[position]->selected) info->states |= XmACCESSIBLE_STATE(23);
  if (focused_widget(a) == (Widget)list && list->list.CurrentKbdItem == position) info->states |= XmACCESSIBLE_STATE(12);
  if (XtIsRealized((Widget)list) && XmListPosToBounds((Widget)list, position + 1, &x, &y, &width, &height)) {
    XtTranslateCoords((Widget)list, x, y, &sx, &sy);
    info->has_extents = True; info->screen_x = sx; info->screen_y = sy;
    info->width = width; info->height = height;
    if (native_region_showing((Widget)list, x, y,
                              (int64_t)x + width, (int64_t)y + height))
      info->states |= XmACCESSIBLE_STATE(25);
  }
  return True;
}
static Boolean query_impl(XmAccessibility *a, XmAccessibleId id,
                             XmAccessibleInfo *i) {
  XmAccessibleNode *n = _XmAccessibilityNode(a, id);
  Widget w;
  XmString label = NULL;
  char *local = NULL;
  Boolean editable = False, checked = False;
  memset(i, 0, sizeof(*i));
  i->rows = i->columns = -1;
  if (!n)
    return False;
  if (id == 1) {
    i->role = XmACCESSIBLE_APPLICATION;
    i->name = XtNewString(a->name);
    return True;
  }
  if (n->list_owner) return native_item_query(a, n, i);
  if (n->provider.query) {
    Boolean result = n->provider.query(n->data, i);
    if (result && i->has_selection && (!n->provider.selection_count ||
        !n->provider.selected_child || !n->provider.child_selected)) result = False;
    if (result && i->has_value &&
        (!isfinite(i->minimum_value) || !isfinite(i->maximum_value) ||
         !isfinite(i->current_value) || !isfinite(i->minimum_increment) ||
         i->minimum_value > i->maximum_value || i->minimum_increment < 0 ||
         i->current_value < i->minimum_value ||
         i->current_value > i->maximum_value))
      result = False;
    if (_XmAccessibilityNode(a, id) != n) result = False;
    if (!result)
      XmAccessibleInfoClear(i);
    return result;
  }
  w = n->widget;
  if (!w) {
    i->states = XmACCESSIBLE_STATE(6);
    i->name = XtNewString("");
    return True;
  }
  if (XtIsRealized(w)) {
    Position x = 0, y = 0, screen_x = 0, screen_y = 0;
    Dimension width = 0, height = 0;
    XtVaGetValues(w, XmNx, &x, XmNy, &y, XmNwidth, &width, XmNheight, &height,
                  NULL);
    if (XtIsWidget(w))
      XtTranslateCoords(w, 0, 0, &screen_x, &screen_y);
    else
      XtTranslateCoords(XtParent(w), x, y, &screen_x, &screen_y);
    i->screen_x = screen_x;
    i->screen_y = screen_y;
    i->width = width;
    i->height = height;
    i->has_extents = True;
  }
  i->role = XtIsShell(w) ? XmACCESSIBLE_FRAME : XmACCESSIBLE_PANEL;
  if (XtIsShell(w) && active_shell(a) == w)
    i->states |= XmACCESSIBLE_STATE(1); /* active: actual WM/input ownership */
  i->states |= XmACCESSIBLE_STATE(30);
  if (XtIsSensitive(w))
    i->states |= XmACCESSIBLE_STATE(8) | XmACCESSIBLE_STATE(24);
  if (i->has_extents && native_region_showing(w, 0, 0, i->width, i->height))
    i->states |= XmACCESSIBLE_STATE(25);
  if (focused_identity(a) == id)
    i->states |= XmACCESSIBLE_STATE(12);
  if (XmIsList(w)) {
    XmListWidget list = (XmListWidget)w;
    i->role = XmACCESSIBLE_LIST;
    i->has_selection = sync_native_list(a, n);
    if (list->primitive.traversal_on) i->states |= XmACCESSIBLE_STATE(11);
    if (list->list.SelectionPolicy == XmMULTIPLE_SELECT || list->list.SelectionPolicy == XmEXTENDED_SELECT)
      i->states |= XmACCESSIBLE_STATE(18);
  }
  if (XmIsLabel(w) || XmIsLabelGadget(w)) {
    i->role = XmACCESSIBLE_LABEL;
    XtVaGetValues(w, XmNlabelString, &label, NULL);
    if (label) {
      local = XmStringUnparse(label, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT, NULL,
                              0, XmOUTPUT_ALL);
      /* Synthetic Label/LabelGadget getters return an owned copy. */
      XmStringFree(label);
      label = NULL;
    }
  }
  if (XmIsPushButton(w) || XmIsPushButtonGadget(w) || XmIsArrowButton(w)) {
    i->role = XmACCESSIBLE_BUTTON;
    i->action_count = 1;
    i->states |= XmACCESSIBLE_STATE(11);
  }
  if (XmIsToggleButton(w) || XmIsToggleButtonGadget(w)) {
    i->role = XmACCESSIBLE_CHECK_BOX;
    i->action_count = 1;
    i->states |= XmACCESSIBLE_STATE(11) | XmACCESSIBLE_STATE(41);
    XtVaGetValues(w, XmNset, &checked, NULL);
    if (checked)
      i->states |= XmACCESSIBLE_STATE(4);
  }
  if (XmIsText(w) || XmIsTextField(w)) {
    i->role = XmACCESSIBLE_TEXT;
    i->states |= XmACCESSIBLE_STATE(11) | XmACCESSIBLE_STATE(38);
    XtVaGetValues(w, XmNeditable, &editable, NULL);
    if (editable)
      i->states |= XmACCESSIBLE_STATE(7);
    i->has_text = True;
    i->text_length = (int)(XmIsText(w) ? XmTextGetLastPosition(w)
                                       : XmTextFieldGetLastPosition(w));
    i->caret = (int)(XmIsText(w) ? XmTextGetInsertionPosition(w)
                                 : XmTextFieldGetInsertionPosition(w));
  }
  if (XmIsScrollBar(w)) {
    int minimum, maximum, value, size, increment, page;
    unsigned char orientation;
    Boolean traversal;
    XmScrollBarGetValues(w, &value, &size, &increment, &page);
    XtVaGetValues(w, XmNminimum, &minimum, XmNmaximum, &maximum, XmNorientation,
                  &orientation, XmNeditable, &editable, XmNtraversalOn,
                  &traversal, NULL);
    i->role = XmACCESSIBLE_SCROLL_BAR;
    i->has_value = True;
    i->minimum_value = minimum;
    i->maximum_value = maximum - size;
    i->current_value = value;
    i->minimum_increment = increment;
    i->states |= XmACCESSIBLE_STATE(orientation == XmHORIZONTAL ? 14 : 29);
    if (traversal)
      i->states |= XmACCESSIBLE_STATE(11);
    if (editable)
      i->states |= XmACCESSIBLE_STATE(7);
    i->action_count = editable ? 6 : 0;
  }
  if (menu_pane(w)) {
    unsigned char type;
    XtVaGetValues(w, XmNrowColumnType, &type, NULL);
    i->role = type == XmMENU_BAR ? XmACCESSIBLE_MENU_BAR : XmACCESSIBLE_MENU;
  }
  if (menu_pane(XtParent(w))) {
    Boolean traversal = False;
    if (XmIsPushButton(w) || XmIsPushButtonGadget(w))
      i->role = XmACCESSIBLE_MENU_ITEM;
    if (XmIsToggleButton(w) || XmIsToggleButtonGadget(w)) {
      unsigned char indicator;
      XtVaGetValues(w, XmNindicatorType, &indicator, NULL);
      i->role = indicator == XmONE_OF_MANY || indicator == XmONE_OF_MANY_ROUND ||
                        indicator == XmONE_OF_MANY_DIAMOND
                    ? XmACCESSIBLE_RADIO_MENU_ITEM : XmACCESSIBLE_CHECK_MENU_ITEM;
    }
    if (XmIsCascadeButton(w) || XmIsCascadeButtonGadget(w)) {
      Widget submenu = NULL;
      i->role = XmACCESSIBLE_MENU_ITEM;
      i->action_count = 1;
      XtVaGetValues(w, XmNsubMenuId, &submenu, NULL);
      if (submenu) {
        i->states |= XmACCESSIBLE_STATE(9) | XmACCESSIBLE_STATE(42);
        if (submenu_posted(w, submenu)) i->states |= XmACCESSIBLE_STATE(10);
      }
    }
    if (i->action_count) {
      XtVaGetValues(w, XmNtraversalOn, &traversal, NULL);
      if (traversal) i->states |= XmACCESSIBLE_STATE(11);
      else i->states &= ~XmACCESSIBLE_STATE(11);
    }
  }
  {
    Widget section = XmIsDisclosure(w)
                         ? w
                         : (XtParent(w) && XmIsDisclosure(XtParent(w)) &&
                                    w != XmDisclosureGetContent(XtParent(w))
                                ? XtParent(w)
                                : NULL);
    if (section) {
      XtFree(local);
      local = NULL;
      /* Disclosure's ordinary resource is borrowed, unlike Label's
       * synthetic getter above. Convert it without taking ownership. */
      XtVaGetValues(section, XmNlabelString, &label, NULL);
      if (label)
        local = XmStringUnparse(label, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
                                NULL, 0, XmOUTPUT_ALL);
      i->states |= XmACCESSIBLE_STATE(9); /* expandable */
      if (XmDisclosureGetExpanded(section))
        i->states |= XmACCESSIBLE_STATE(10);
      i->action_count = 1;
    }
  }
  i->name = locale_utf8(local ? local : XtName(w));
  XtFree(local);
  if (!i->name)
    i->name = XtNewString("");
  return True;
}
static char *text_impl(XmAccessibility *a, XmAccessibleId id, int start,
                          int end) {
  XmAccessibleNode *n = _XmAccessibilityNode(a, id);
  XmAccessibleInfo info;
  char *local, *result;
  size_t capacity;
  int status;
  if (!n || start < 0 || end < start || !XmAccessibilityQuery(a, id, &info))
    return NULL;
  if (!info.has_text || info.role == XmACCESSIBLE_PASSWORD_TEXT ||
      end > info.text_length) {
    XmAccessibleInfoClear(&info);
    return NULL;
  }
  XmAccessibleInfoClear(&info);
  if (start == end)
    return XtNewString("");
  if (n->provider.text) {
    result = n->provider.text(n->data, start, end);
    if (_XmAccessibilityNode(a, id) != n || a->destroy_requested) {
      XtFree(result);
      return NULL;
    }
    return result;
  }
  if (!n->widget || (!XmIsText(n->widget) && !XmIsTextField(n->widget)))
    return NULL;
  if ((size_t)(end - start) > (INT_MAX - 1) / MB_CUR_MAX)
    return NULL;
  capacity = (size_t)(end - start) * MB_CUR_MAX + 1;
  local = XtMalloc((Cardinal)capacity);
  status = XmIsText(n->widget)
               ? XmTextGetSubstring(n->widget, start, end - start,
                                    (int)capacity, local)
               : XmTextFieldGetSubstring(n->widget, start, end - start,
                                         (int)capacity, local);
  if (status != XmCOPY_SUCCEEDED) {
    XtFree(local);
    return NULL;
  }
  result = locale_utf8(local);
  XtFree(local);
  return result;
}
/* Semantic providers have no implicit application-wide native owner. Resolve
 * live ancestry for every admission, including after callbacks that may remove
 * nodes, destroy widgets or change the modal cascade. Retired node storage is
 * retained by the surrounding dispatch guard, never evidence of liveness. */
static Boolean input_allowed(XmAccessibility *a, XmAccessibleId id,
                             XmAccessibleNode *expected) {
  XmAccessibleNode *n;
  if (a->destroy_requested || !(n = _XmAccessibilityNode(a, id)) || n != expected)
    return False;
  while (n) {
    Widget owner = n->widget ? n->widget : n->list_owner;
    if (owner) return _XmModalInputAllowed(owner);
    n = _XmAccessibilityNode(a, n->parent);
  }
  return False;
}
static Boolean action_impl(XmAccessibility *a, XmAccessibleId id,
                              unsigned int action) {
  XmAccessibleNode *n = _XmAccessibilityNode(a, id);
  if (!n || !input_allowed(a, id, n))
    return False;
  if (n->list_owner) {
    if (action || native_item_position(a, n) < 0 || !_XmAccessibilityNode(a, n->parent)->list_sync_valid) return False;
    return _XmListChangeSelection(n->list_owner, 0, n->list_token);
  }
  if (n->provider.action)
    return n->provider.action(n->data, action);
  if (n->widget && XmIsScrollBar(n->widget)) {
    XmAccessibleInfo info;
    int value, size, increment, page;
    double next;
    if (action >= 6 || !XmAccessibilityQuery(a, id, &info))
      return False;
    XmScrollBarGetValues(n->widget, &value, &size, &increment, &page);
    next = action == 0   ? (double)value + increment
           : action == 1 ? (double)value - increment
           : action == 2 ? (double)value + page
           : action == 3 ? (double)value - page
           : action == 4 ? info.minimum_value
                         : info.maximum_value;
    if (next < info.minimum_value)
      next = info.minimum_value;
    if (next > info.maximum_value)
      next = info.maximum_value;
    XmAccessibleInfoClear(&info);
    return XmAccessibilitySetValue(a, id, next);
  }
  if (!n->widget || action || !XtIsSensitive(n->widget))
    return False;
  if (menu_pane(XtParent(n->widget))) {
    XtActionProc activate = NULL;
    Cardinal count = 0;
    if (XmIsPrimitive(n->widget))
      activate = ((XmPrimitiveWidgetClass)XtClass(n->widget))->primitive_class.arm_and_activate;
    else if (XmIsGadget(n->widget))
      activate = ((XmGadgetClass)XtClass(n->widget))->gadget_class.arm_and_activate;
    /* Use the native menu activation contract: callbacks alone leave menu
     * grabs, focus and posted panes behind. Labels have no activation method. */
    if (!activate) return False;
    activate(n->widget, NULL, NULL, &count);
    if (!a->destroy_requested) {
      notify_focus(a);
      if (_XmAccessibilityNode(a, id)) XmAccessibilityChanged(a, id);
    }
    return True;
  }
  if (XmIsDisclosure(n->widget)) {
    XmDisclosureSetExpanded(n->widget, !XmDisclosureGetExpanded(n->widget),
                            True);
    return True;
  }
  if (XmIsArrowButton(n->widget)) {
    XmArrowButtonCallbackStruct cb;
    cb.reason = XmCR_ACTIVATE;
    cb.event = NULL;
    cb.click_count = 1;
    XtCallCallbacks(n->widget, XmNactivateCallback, &cb);
    return True;
  }
  if (XmIsToggleButton(n->widget)) {
    XmToggleButtonSetState(n->widget, !XmToggleButtonGetState(n->widget), True);
    return True;
  }
  if (XmIsToggleButtonGadget(n->widget)) {
    XmToggleButtonGadgetSetState(
        n->widget, !XmToggleButtonGadgetGetState(n->widget), True);
    return True;
  }
  if (!XmIsPushButton(n->widget) && !XmIsPushButtonGadget(n->widget))
    return False;
  {
    XmPushButtonCallbackStruct cb;
    /* One semantic activation has no native input event or multi-click
     * sequence. Preserve the full native callback contract for both classes. */
    cb.reason = XmCR_ACTIVATE;
    cb.event = NULL;
    cb.click_count = 1;
    XtCallCallbacks(n->widget, XmNactivateCallback, &cb);
  }
  return True;
}
static char *action_name_impl(XmAccessibility *a, XmAccessibleId id,
                                unsigned int action) {
  static const char *names[] = {"increment",      "decrement",
                                "page-increment", "page-decrement",
                                "to-minimum",     "to-maximum"};
  XmAccessibleNode *n = _XmAccessibilityNode(a, id);
  XmAccessibleInfo info;
  if (!n || !XmAccessibilityQuery(a, id, &info))
    return NULL;
  if (action >= info.action_count) {
    XmAccessibleInfoClear(&info);
    return NULL;
  }
  XmAccessibleInfoClear(&info);
  if (n->list_owner) return XtNewString("select");
  if (n->provider.action_name)
    return n->provider.action_name(n->data, action);
  {
    const char *name =
        n->widget && XmIsScrollBar(n->widget) ? names[action] : "activate";
    return XtNewString(name);
  }
}
static Boolean set_value_impl(XmAccessibility *a, XmAccessibleId id,
                                double value) {
  XmAccessibleNode *n = _XmAccessibilityNode(a, id);
  XmAccessibleInfo info;
  Boolean allowed;
  if (!n || !input_allowed(a, id, n) || !isfinite(value) ||
      !XmAccessibilityQuery(a, id, &info))
    return False;
  allowed = info.has_value && isfinite(info.minimum_value) &&
            isfinite(info.maximum_value) && value >= info.minimum_value &&
            value <= info.maximum_value &&
            (info.states & XmACCESSIBLE_STATE(8)) &&
            (info.states & XmACCESSIBLE_STATE(7));
  XmAccessibleInfoClear(&info);
  if (!allowed || !input_allowed(a, id, n))
    return False;
  if (n->provider.set_value)
    return n->provider.set_value(n->data, value);
  if (!n->widget || !XmIsScrollBar(n->widget) || value < INT_MIN ||
      value > INT_MAX || value != (double)(int)value)
    return False;
  XmScrollBarSetValues(n->widget, (int)value, 0, 0, 0, True);
  return True;
}
static Boolean focus_impl(XmAccessibility *a, XmAccessibleId id) {
  XmAccessibleNode *n = _XmAccessibilityNode(a, id);
  if (!n || !input_allowed(a, id, n))
    return False;
  if (n->list_owner) {
    int position = native_item_position(a, n);
    Widget owner = n->list_owner;
    uint64_t token = n->list_token;
    if (position < 0 || !XtIsSensitive(owner)) return False;
    if (!XmProcessTraversal(owner, XmTRAVERSE_CURRENT)) return False;
    /* Traversal may execute focus callbacks. Re-resolve the exact token. */
    n = _XmAccessibilityNode(a, id);
    if (!n || n->list_token != token || !input_allowed(a, id, n) ||
        (position = native_item_position(a, n)) < 0) return False;
    if (!XmListSetKbdItemPos(owner, position + 1)) return False;
    if (!input_allowed(a, id, n)) return False;
    notify_focus(a); return True;
  }
  if (n->provider.focus)
    return n->provider.focus(n->data);
  if (n->widget) {
    Widget target = XmIsDisclosure(n->widget) ? XmDisclosureGetHeader(n->widget)
                                              : n->widget;
    Boolean success = XmProcessTraversal(target, XmTRAVERSE_CURRENT);
    if (!input_allowed(a, id, n)) return False;
    notify_focus(a);
    return success;
  }
  return False;
}
void XmAccessibilityChanged(XmAccessibility *a, XmAccessibleId id) {
  if (!a || !id) return;
  ++a->dispatch_depth;
  {
    XmAccessibleNode *node = _XmAccessibilityNode(a, id);
    if (node && node->change_serial != UINT64_MAX) ++node->change_serial;
  }
  _XmAccessibilityTransportChanged(a, id);
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
}
XmAccessibilityStatus XmAccessibilityConnect(XmAccessibility *a) {
  return _XmAccessibilityTransportConnect(a);
}
XmAccessibilityStatus XmAccessibilityGetStatus(XmAccessibility *a) {
  return a->status;
}
const char *XmAccessibilityBusName(XmAccessibility *a) {
  return _XmAccessibilityTransportBusName(a);
}
Boolean XmAccessibilityQuery(XmAccessibility *a, XmAccessibleId id, XmAccessibleInfo *info) {
  Boolean result;
  if (!a) return False;
  ++a->dispatch_depth;
  result = query_impl(a, id, info);
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
  return result;
}

char *XmAccessibilityText(XmAccessibility *a, XmAccessibleId id, int start, int end) {
  char * result;
  if (!a) return NULL;
  ++a->dispatch_depth;
  result = text_impl(a, id, start, end);
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
  return result;
}

Boolean XmAccessibilityAction(XmAccessibility *a, XmAccessibleId id, unsigned int action) {
  Boolean result;
  if (!a) return False;
  ++a->dispatch_depth;
  result = action_impl(a, id, action);
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
  return result;
}

char *XmAccessibilityActionName(XmAccessibility *a, XmAccessibleId id, unsigned int action) {
  char * result;
  if (!a) return NULL;
  ++a->dispatch_depth;
  result = action_name_impl(a, id, action);
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
  return result;
}

Boolean XmAccessibilitySetValue(XmAccessibility *a, XmAccessibleId id, double value) {
  Boolean result;
  if (!a) return False;
  ++a->dispatch_depth;
  result = set_value_impl(a, id, value);
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
  return result;
}

Boolean XmAccessibilityFocus(XmAccessibility *a, XmAccessibleId id) {
  Boolean result;
  if (!a) return False;
  ++a->dispatch_depth;
  result = focus_impl(a, id);
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
  return result;
}

XmAccessibleId XmAccessibilityAdd(XmAccessibility *a, XmAccessibleId parent, const XmAccessibleProvider *provider, XtPointer data) {
  XmAccessibleId result;
  if (!a) return 0;
  ++a->dispatch_depth;
  result = add_impl(a, parent, provider, data);
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
  return result;
}

static XmAccessibleNode *selection_owner(XmAccessibility *a, XmAccessibleId id) {
  XmAccessibleInfo info;
  XmAccessibleNode *n;
  if (a->destroy_requested || !XmAccessibilityQuery(a, id, &info)) return NULL;
  if (!info.has_selection) { XmAccessibleInfoClear(&info); return NULL; }
  XmAccessibleInfoClear(&info);
  n = _XmAccessibilityNode(a, id);
  return n;
}
static Boolean selection_stable(XmAccessibility *a, XmAccessibleNode *n,
    XmAccessibleId id, uint64_t children_serial, uint64_t change_serial) {
  return !a->destroy_requested && _XmAccessibilityNode(a, id) == n &&
      children_serial != UINT64_MAX && change_serial != UINT64_MAX &&
      n->children_serial == children_serial && n->change_serial == change_serial;
}
int XmAccessibilitySelectionCount(XmAccessibility *a, XmAccessibleId id) {
  XmAccessibleNode *n;
  uint64_t children_serial, change_serial;
  int count = -1;
  if (!a) return -1;
  ++a->dispatch_depth;
  n = selection_owner(a, id);
  if (!n) goto done;
  children_serial = n->children_serial; change_serial = n->change_serial;
  if (!selection_stable(a, n, id, children_serial, change_serial)) goto done;
  if (n->widget && XmIsList(n->widget)) count = (int)n->list_selected_count;
  else if (n->provider.selection_count) count = n->provider.selection_count(n->data);
  if (!selection_stable(a, n, id, children_serial, change_serial) ||
      count < 0 || (size_t)count > n->child_count) count = -1;
done:
  --a->dispatch_depth; _XmAccessibilityDispatchDone(a); return count;
}
XmAccessibleId XmAccessibilitySelectedChild(XmAccessibility *a, XmAccessibleId id, int index) {
  XmAccessibleNode *n, *child;
  uint64_t children_serial, change_serial;
  XmAccessibleId result = 0;
  if (!a || index < 0) return 0;
  ++a->dispatch_depth;
  n = selection_owner(a, id);
  if (!n) goto done;
  children_serial = n->children_serial; change_serial = n->change_serial;
  if (index >= XmAccessibilitySelectionCount(a, id) ||
      !selection_stable(a, n, id, children_serial, change_serial)) goto done;
  if (n->widget && XmIsList(n->widget)) result = n->list_selected[index];
  else if (n->provider.selected_child) result = n->provider.selected_child(n->data, index);
  child = _XmAccessibilityNode(a, result);
  if (!selection_stable(a, n, id, children_serial, change_serial) ||
      !child || child->parent != id) result = 0;
done:
  --a->dispatch_depth; _XmAccessibilityDispatchDone(a); return result;
}
Boolean XmAccessibilityChildSelected(XmAccessibility *a, XmAccessibleId id, XmAccessibleId child_id) {
  XmAccessibleNode *n, *child;
  uint64_t children_serial, change_serial;
  Boolean result = False;
  if (!a) return False;
  ++a->dispatch_depth;
  n = selection_owner(a, id); child = _XmAccessibilityNode(a, child_id);
  if (!n || !child || child->parent != id) goto done;
  children_serial = n->children_serial; change_serial = n->change_serial;
  if (!selection_stable(a, n, id, children_serial, change_serial)) goto done;
  if (n->widget && XmIsList(n->widget)) {
    int pos = native_item_position(a, child);
    if (pos >= 0) result = ((XmListWidget)n->widget)->list.InternalList[pos]->selected;
  } else if (n->provider.child_selected) result = n->provider.child_selected(n->data, child_id);
  if (!selection_stable(a, n, id, children_serial, change_serial) ||
      _XmAccessibilityNode(a, child_id) != child) result = False;
done:
  --a->dispatch_depth; _XmAccessibilityDispatchDone(a); return result;
}
/* Provider reads can reenter the bridge. Every callback boundary validates the
   exact hierarchy and semantic epoch; selected IDs form a unique coherent set. */
static Boolean selection_snapshot(XmAccessibility *a, XmAccessibleNode *n,
    XmAccessibleId id, XmAccessibleId *ids, unsigned char *seen,
    size_t capacity, int *count) {
  uint64_t children_serial = n->children_serial, change_serial = n->change_serial;
  int length, final_length, k;
  if (!selection_stable(a, n, id, children_serial, change_serial) ||
      n->child_count != capacity || !n->provider.selection_count ||
      !n->provider.selected_child) return False;
  length = n->provider.selection_count(n->data);
  if (!selection_stable(a, n, id, children_serial, change_serial) ||
      length < 0 || (size_t)length > capacity) return False;
  memset(seen, 0, capacity);
  for (k = 0; k < length; ++k) {
    XmAccessibleNode *child;
    ids[k] = n->provider.selected_child(n->data, k);
    if (!selection_stable(a, n, id, children_serial, change_serial)) return False;
    child = _XmAccessibilityNode(a, ids[k]);
    if (!child || child->parent != id || child->child_index >= capacity ||
        seen[child->child_index]) return False;
    seen[child->child_index] = 1;
  }
  final_length = n->provider.selection_count(n->data);
  if (!selection_stable(a, n, id, children_serial, change_serial) ||
      final_length != length) return False;
  *count = length;
  return True;
}
Boolean XmAccessibilityChangeSelection(XmAccessibility *a, XmAccessibleId id,
    XmAccessibleSelectionOperation operation, XmAccessibleId child_id) {
  XmAccessibleNode *n, *child = NULL;
  XmAccessibleInfo info;
  XmAccessibleId *before = NULL, *after = NULL;
  unsigned char *seen = NULL;
  size_t capacity;
  uint64_t children_serial;
  int count, next_count;
  Boolean result = False;
  if (!a || operation < XmACCESSIBLE_SELECT_CHILD || operation > XmACCESSIBLE_CLEAR_SELECTION) return False;
  ++a->dispatch_depth;
  n = selection_owner(a, id);
  if (!n || !input_allowed(a, id, n) || !XmAccessibilityQuery(a, id, &info)) goto done;
  result = (info.states & XmACCESSIBLE_STATE(8)) && (info.states & XmACCESSIBLE_STATE(24));
  XmAccessibleInfoClear(&info);
  if (!result || !input_allowed(a, id, n)) { result = False; goto done; }
  result = False;
  if (operation < XmACCESSIBLE_SELECT_ALL) {
    child = _XmAccessibilityNode(a, child_id);
    if (!child || child->parent != id) goto done;
  } else if (child_id) goto done;
  if (n->widget && XmIsList(n->widget)) {
    if (child && native_item_position(a, child) < 0) goto done;
    result = _XmListChangeSelection(n->widget, (unsigned int)operation, child ? child->list_token : 0);
    goto done;
  }
  if (!n->provider.change_selection) goto done;
  capacity = n->child_count;
  before = calloc(capacity ? capacity : 1, sizeof(*before));
  after = calloc(capacity ? capacity : 1, sizeof(*after));
  seen = calloc(capacity ? capacity : 1, sizeof(*seen));
  children_serial = n->children_serial;
  if (!before || !after || !seen ||
      !selection_snapshot(a, n, id, before, seen, capacity, &count)) goto done;
  if (child && _XmAccessibilityNode(a, child_id) != child) goto done;
  if (!input_allowed(a, id, n)) goto done;
  result = n->provider.change_selection(n->data, operation, child_id);
  if (!result || a->destroy_requested || _XmAccessibilityNode(a, id) != n) { result = False; goto done; }
  if (n->children_serial != children_serial ||
      !selection_snapshot(a, n, id, after, seen, capacity, &next_count)) {
    result = False; goto done;
  }
  if (count != next_count || (count && memcmp(before, after, (size_t)count * sizeof(*before)))) {
    XmAccessibilityChanged(a, id);
    if (_XmAccessibilityNode(a, id) == n) _XmAccessibilityTransportSelectionChanged(a, id);
  }
done:
  free(before); free(after); free(seen);
  --a->dispatch_depth; _XmAccessibilityDispatchDone(a); return result;
}
Boolean _XmAccessibilityNativeTextSelection(XmAccessibility *a, XmAccessibleId id,
                                            Boolean *selected, int *first, int *last) {
  XmAccessibleNode *n = _XmAccessibilityNode(a, id);
  XmTextPosition start = 0, end = 0;
  Boolean value;
  if (!n || !n->widget || !selected || !first || !last) return False;
  if (XmIsText(n->widget)) value = XmTextGetSelectionPosition(n->widget, &start, &end);
  else if (XmIsTextField(n->widget)) value = XmTextFieldGetSelectionPosition(n->widget, &start, &end);
  else return False;
  if (value && (start < 0 || end < start || end > INT_MAX)) return False;
  *selected = value; *first = value ? (int)start : 0; *last = value ? (int)end : 0;
  return True;
}
Boolean XmAccessibilityVisualLine(XmAccessibility *a, XmAccessibleId id, int offset,
                                  int direction, int *first, int *last) {
  XmAccessibleNode *n;
  XmAccessibleInfo info;
  Boolean result = False;
  if (!a || !first || !last || offset < 0 || direction < -1 || direction > 1) return False;
  ++a->dispatch_depth;
  n = _XmAccessibilityNode(a, id);
  if (!n || !XmAccessibilityQuery(a, id, &info)) goto done;
  if (!info.has_text || offset > info.text_length) { XmAccessibleInfoClear(&info); goto done; }
  {
    int length = info.text_length;
    XmAccessibleInfoClear(&info);
    if (n->provider.visual_line) {
      result = n->provider.visual_line(n->data, offset, direction, first, last);
      if (result && (_XmAccessibilityNode(a, id) != n || *first < 0 || *last < *first || *last > length)) result = False;
    } else if (n->widget && XmIsTextField(n->widget)) {
      *first = direction > 0 ? length : 0;
      *last = direction < 0 ? 0 : length;
      result = True;
    } else if (n->widget && XmIsText(n->widget)) {
      XmTextWidget text = (XmTextWidget)n->widget;
      XmTextLineTable lines = text->text.line_table;
      int count = text->text.total_lines, lower = 0, upper = count, index;
      if (!lines || count <= 0) goto done;
      while (lower < upper) {
        int middle = lower + (upper - lower) / 2;
        if (lines[middle].start_pos <= (unsigned int)offset) lower = middle + 1;
        else upper = middle;
      }
      index = lower - 1 + direction;
      if (index < 0) *first = *last = 0;
      else if (index >= count) *first = *last = length;
      else {
        *first = (int)lines[index].start_pos;
        *last = index + 1 < count ? (int)lines[index + 1].start_pos : length;
      }
      result = *first >= 0 && *last >= *first && *last <= length;
    }
  }
done:
  --a->dispatch_depth;
  _XmAccessibilityDispatchDone(a);
  return result;
}
#ifndef HAVE_ATSPI_DBUS
void _XmAccessibilityTransportDestroy(XmAccessibility *a) { (void)a; }
XmAccessibilityStatus _XmAccessibilityTransportConnect(XmAccessibility *a) {
  return a->status = XmACCESSIBILITY_NOT_BUILT;
}
void _XmAccessibilityTransportSelectionChanged(XmAccessibility *a, XmAccessibleId id) { (void)a; (void)id; }
void _XmAccessibilityTransportChanged(XmAccessibility *a, XmAccessibleId id) {
  (void)a;
  (void)id;
}
Boolean _XmAccessibilityTransportKey(XmAccessibility *a, const XKeyEvent *event,
                                      KeySym symbol, const char *label,
                                      Boolean is_text) {
  (void)a; (void)event; (void)symbol; (void)label; (void)is_text;
  return False;
}
void _XmAccessibilityTransportRemoved(XmAccessibility *a, XmAccessibleId id,
                                     XmAccessibleId parent, int index) {
  (void)a; (void)id; (void)parent; (void)index;
}
const char *_XmAccessibilityTransportBusName(XmAccessibility *a) {
  (void)a;
  return "";
}
#endif
