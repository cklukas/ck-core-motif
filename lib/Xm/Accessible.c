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
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

XmAccessibleNode *_XmAccessibilityNode(XmAccessibility *a, XmAccessibleId id) {
  XmAccessibleNode *n;
  for (n = a->nodes; n; n = n->next)
    if (n->id == id)
      return n;
  return NULL;
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
  return shell_for(XtWindowToWidget(display, focus));
}
static void node_destroyed(Widget w, XtPointer data, XtPointer call) {
  XmAccessibility *a = (XmAccessibility *)data;
  XmAccessibleNode *n;
  (void)call;
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
      n->widget = NULL;
      XmAccessibilityRemove(a, id);
      XmAccessibilityChanged(a, 1);
    }
  }
}
static void notify_focus(XmAccessibility *a) {
  Widget shell = active_shell(a);
  XmAccessibleId next =
      shell ? XmAccessibilityWidget(a, XmGetFocusWidget(shell)) : 0;
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
  notify_focus(a);
  XmAccessibilityChanged(a, XmAccessibilityWidget(a, w));
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
  XmAccessibilityChanged(a, parent);
  for (n = a->nodes; n; n = n->next)
    if (n->parent == parent && n->widget != XmDisclosureGetContent(w))
      XmAccessibilityChanged(a, n->id);
}
static XmAccessibleNode *add_node(XmAccessibility *a, XmAccessibleId parent) {
  XmAccessibleNode *n;
  if (a->next_id == UINT32_MAX)
    return NULL;
  n = (XmAccessibleNode *)XtCalloc(1, sizeof(*n));
  n->id = ++a->next_id;
  n->parent = parent;
  n->next = a->nodes;
  a->nodes = n;
  return n;
}
XmAccessibleId XmAccessibilityWidget(XmAccessibility *a, Widget w) {
  XmAccessibleNode *n;
  if (!a || !w)
    return 0;
  for (n = a->nodes; n; n = n->next)
    if (n->widget == w)
      return n->id;
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
    n->widget = w;
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
  (void)n;
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
  XmAccessibleNode *n;
  if (!a)
    return;
  for (n = a->nodes; n; n = n->next)
    if (n->parent == 1 && n->widget)
      add_widgets(a, n->widget, 1);
}
XmAccessibleId XmAccessibilityRoot(XmAccessibility *a) { return a ? 1 : 0; }
XmAccessibleId XmAccessibilityAdd(XmAccessibility *a, XmAccessibleId parent,
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
  XmAccessibilityChanged(a, parent);
  return n->id;
}
void XmAccessibilityRemove(XmAccessibility *a, XmAccessibleId id) {
  XmAccessibleNode **p = &a->nodes, *n;
  if (id <= 1)
    return;
  /* Remove descendants before their provider-owned parent data is released. */
  for (;;) {
    for (n = a->nodes; n; n = n->next)
      if (n->parent == id)
        break;
    if (!n)
      break;
    XmAccessibilityRemove(a, n->id);
  }
  while (*p && (*p)->id != id)
    p = &(*p)->next;
  if (!*p)
    return;
  n = *p;
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
  *p = n->next;
  XtFree((char *)n);
}
static void deferred_destroy(XtPointer data, XtIntervalId *id) {
  XmAccessibility *a = (XmAccessibility *)data;
  (void)id;
  a->deferred_destroy = 0;
  XmAccessibilityDestroy(a);
}
void XmAccessibilityDestroy(XmAccessibility *a) {
  XmAccessibleNode *n;
  if (!a)
    return;
  if (a->dispatch_depth) {
    if (!a->deferred_destroy)
      a->deferred_destroy =
          XtAppAddTimeOut(a->app, 0, deferred_destroy, (XtPointer)a);
    return;
  }
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
      XtFree((char *)n);
    } else
      XmAccessibilityRemove(a, n->id);
  }
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
Boolean XmAccessibilityQuery(XmAccessibility *a, XmAccessibleId id,
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
  i->states = XmACCESSIBLE_STATE(30);
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
char *XmAccessibilityText(XmAccessibility *a, XmAccessibleId id, int start,
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
Boolean XmAccessibilityAction(XmAccessibility *a, XmAccessibleId id,
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
char *XmAccessibilityActionName(XmAccessibility *a, XmAccessibleId id,
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
Boolean XmAccessibilitySetValue(XmAccessibility *a, XmAccessibleId id,
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
Boolean XmAccessibilityFocus(XmAccessibility *a, XmAccessibleId id) {
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
  if (a && id)
    _XmAccessibilityTransportChanged(a, id);
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
#ifndef HAVE_ATSPI_DBUS
void _XmAccessibilityTransportDestroy(XmAccessibility *a) { (void)a; }
XmAccessibilityStatus _XmAccessibilityTransportConnect(XmAccessibility *a) {
  return a->status = XmACCESSIBILITY_NOT_BUILT;
}
void _XmAccessibilityTransportChanged(XmAccessibility *a, XmAccessibleId id) {
  (void)a;
  (void)id;
}
const char *_XmAccessibilityTransportBusName(XmAccessibility *a) {
  (void)a;
  return "";
}
#endif
