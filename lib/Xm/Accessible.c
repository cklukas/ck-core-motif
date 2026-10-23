/* Generic widget semantics and custom semantic providers. LGPL-2.0-or-later. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "AccessibleI.h"
#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <Xm/ArrowB.h>
#include <Xm/DisclosureP.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
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

static void deferred_destroy(XtPointer, XtIntervalId *);

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
  return n ? n->child_count : 0;
}
XmAccessibleId _XmAccessibilityChildAt(XmAccessibility *a, XmAccessibleId id, size_t index) {
  XmAccessibleNode *n = _XmAccessibilityNode(a, id);
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
    free(n);
  }
  if (a->destroy_requested && !a->deferred_destroy)
    a->deferred_destroy = XtAppAddTimeOut(a->app, 0, deferred_destroy, (XtPointer)a);
}
static Widget shell_for(Widget widget) {
  while (widget && !XtIsShell(widget))
    widget = XtParent(widget);
  return widget;
}
static Widget active_shell(XmAccessibility *a) {
  Window focus;
  int revert;
  Display *display;
  if (!a->root)
    return NULL;
  display = XtDisplayOfObject(a->root);
  XGetInputFocus(display, &focus, &revert);
  if (focus == None || focus == PointerRoot)
    return NULL;
  {
    Widget shell = shell_for(XtWindowToWidget(display, focus));
    /* Override/menu shells borrow their owning WM frame's activation. */
    while (shell && !XtIsWMShell(shell)) shell = shell_for(XtParent(shell));
    return shell;
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
  Widget shell = active_shell(a);
  XmAccessibleId active = shell ? XmAccessibilityWidget(a, shell) : 0;
  XmAccessibleId next =
      shell ? XmAccessibilityWidget(a, XmGetFocusWidget(shell)) : 0;
  if (active != a->active_shell_node) {
    XmAccessibleId previous = a->active_shell_node;
    a->active_shell_node = active;
    if (previous) XmAccessibilityChanged(a, previous);
    if (active) XmAccessibilityChanged(a, active);
  }
  if (next != a->focused_node) {
    XmAccessibleId previous = a->focused_node;
    a->focused_node = next;
    if (previous)
      XmAccessibilityChanged(a, previous);
    if (next)
      XmAccessibilityChanged(a, next);
  }
}
static void widget_event(Widget w, XtPointer data, XEvent *event,
                         Boolean *cont) {
  XmAccessibility *a = (XmAccessibility *)data;
  (void)event;
  (void)cont;
  ++a->dispatch_depth;
  notify_focus(a);
  XmAccessibilityChanged(a, XmAccessibilityWidget(a, w));
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
  if (!strcmp(change->type, XtHsetValues) && XmIsScrollBar(change->widget) &&
      (id = XmAccessibilityWidget(a, change->widget)))
    XmAccessibilityChanged(a, id);
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
  if (p) { n->child_index = p->child_count; p->children[p->child_count++] = n; }
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
    XtAddCallback(w, XmNdestroyCallback, node_destroyed, (XtPointer)a);
    if (XmIsDisclosure(w))
      XtAddCallback(w, XmNdisclosureStateCallback, disclosure_changed,
                    (XtPointer)a);
    if (XmIsText(w) || XmIsTextField(w))
      XtAddCallback(w, XmNvalueChangedCallback, text_value_changed,
                    (XtPointer)a);
    if (XtIsWidget(w))
      XtAddEventHandler(w,
                        FocusChangeMask | StructureNotifyMask |
                            PropertyChangeMask | KeyReleaseMask |
                            ButtonReleaseMask,
                        False, widget_event, (XtPointer)a);
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
    if (XtIsWidget(n->widget))
      XtRemoveEventHandler(n->widget,
                           FocusChangeMask | StructureNotifyMask |
                               PropertyChangeMask | KeyReleaseMask |
                               ButtonReleaseMask,
                           False, widget_event, (XtPointer)a);
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
    else free(n);
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
      free(n->children); free(n);
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
  if (n->provider.query) {
    Boolean result = n->provider.query(n->data, i);
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
  if (XtIsRealized(w)) {
    XWindowAttributes attributes;
    if (XGetWindowAttributes(XtDisplayOfObject(w), XtWindowOfObject(w),
                             &attributes) &&
        attributes.map_state == IsViewable && (XtIsShell(w) || XtIsManaged(w)))
      i->states |= XmACCESSIBLE_STATE(25);
  }
  if (shell_for(w) == active_shell(a) && shell_for(w) &&
      XmGetFocusWidget(shell_for(w)) == w)
    i->states |= XmACCESSIBLE_STATE(12);
  if (XmIsLabel(w) || XmIsLabelGadget(w)) {
    i->role = XmACCESSIBLE_LABEL;
    XtVaGetValues(w, XmNlabelString, &label, NULL);
    if (label)
      local = XmStringUnparse(label, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT, NULL,
                              0, XmOUTPUT_ALL);
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
  if (!info.has_text || end > info.text_length) {
    XmAccessibleInfoClear(&info);
    return NULL;
  }
  XmAccessibleInfoClear(&info);
  if (start == end)
    return XtNewString("");
  if (n->provider.text)
    return n->provider.text(n->data, start, end);
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
static Boolean action_impl(XmAccessibility *a, XmAccessibleId id,
                              unsigned int action) {
  XmAccessibleNode *n = _XmAccessibilityNode(a, id);
  XmAnyCallbackStruct cb;
  if (!n)
    return False;
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
  if (XmIsDisclosure(n->widget)) {
    XmDisclosureSetExpanded(n->widget, !XmDisclosureGetExpanded(n->widget),
                            True);
    return True;
  }
  if (XmIsArrowButton(n->widget)) {
    cb.reason = XmCR_ACTIVATE;
    cb.event = NULL;
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
  cb.reason = XmCR_ACTIVATE;
  cb.event = NULL;
  XtCallCallbacks(n->widget, XmNactivateCallback, &cb);
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
  if (!n || !isfinite(value) || !XmAccessibilityQuery(a, id, &info))
    return False;
  allowed = info.has_value && isfinite(info.minimum_value) &&
            isfinite(info.maximum_value) && value >= info.minimum_value &&
            value <= info.maximum_value &&
            (info.states & XmACCESSIBLE_STATE(8)) &&
            (info.states & XmACCESSIBLE_STATE(7));
  XmAccessibleInfoClear(&info);
  if (!allowed)
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
  if (!n)
    return False;
  if (n->provider.focus)
    return n->provider.focus(n->data);
  if (n->widget) {
    Widget target = XmIsDisclosure(n->widget) ? XmDisclosureGetHeader(n->widget)
                                              : n->widget;
    Boolean success = XmProcessTraversal(target, XmTRAVERSE_CURRENT);
    notify_focus(a);
    return success;
  }
  return False;
}
void XmAccessibilityChanged(XmAccessibility *a, XmAccessibleId id) {
  if (!a || !id) return;
  ++a->dispatch_depth;
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
void _XmAccessibilityTransportChanged(XmAccessibility *a, XmAccessibleId id) {
  (void)a;
  (void)id;
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
