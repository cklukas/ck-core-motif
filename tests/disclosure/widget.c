/* Native disclosure geometry, focus and semantic regression test.
 * LGPL-2.0-or-later. */
#include <X11/Shell.h>
#include <X11/keysym.h>
#include <Xm/Accessible.h>
#include <Xm/Disclosure.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
static int notifications;
static Boolean last_expanded;
static Boolean destroyed_in_callback;
static void close_section(Widget w, XtPointer data, XtPointer value) {
  (void)data;
  (void)value;
  destroyed_in_callback = True;
  XtDestroyWidget(w);
}
static void changed(Widget w, XtPointer data, XtPointer value) {
  XmDisclosureCallbackStruct *cb = value;
  (void)w;
  (void)data;
  assert(cb->reason == XmCR_VALUE_CHANGED);
  notifications++;
  last_expanded = cb->expanded;
}
static void pump(XtAppContext app, Display *display) {
  int i;
  for (i = 0; i < 8; i++) {
    struct timespec pause = {0, 5000000};
    XSync(display, False);
    while (XtAppPending(app))
      XtAppProcessEvent(app, XtIMAll);
    nanosleep(&pause, NULL);
  }
}
static void key(Widget target, KeySym symbol) {
  XEvent event;
  memset(&event, 0, sizeof(event));
  event.xkey.type = KeyPress;
  event.xkey.display = XtDisplay(target);
  event.xkey.window = XtWindow(target);
  event.xkey.root = DefaultRootWindow(XtDisplay(target));
  event.xkey.keycode = XKeysymToKeycode(XtDisplay(target), symbol);
  event.xkey.same_screen = True;
  XSendEvent(XtDisplay(target), XtWindow(target), False, KeyPressMask, &event);
}
int main(int argc, char **argv) {
  XtAppContext app;
  Widget shell, column, section, content, header, field, hidden;
  XmString label;
  XmAccessibility *bridge;
  XmAccessibleInfo info;
  XmAccessibleId id;
  XtWidgetGeometry geometry;
  Dimension expanded, collapsed, width, host_width;
  XWindowAttributes attributes;
  Boolean observe = argc == 2 && !strcmp(argv[1], "--observe");
  assert(XmToolkitABI() == XmABI_VERSION);
  shell = XtVaAppInitialize(&app, "DisclosureTest", NULL, 0, &argc, argv, NULL,
                            XmNallowShellResize, True, NULL);
  column = XtVaCreateManagedWidget("column", xmRowColumnWidgetClass, shell,
                                   XmNorientation, XmVERTICAL, NULL);
  label = XmStringCreateLocalized("Section title");
  section = XtVaCreateManagedWidget("section", xmDisclosureWidgetClass, column,
                                    XmNlabelString, label, NULL);
  XmStringFree(label); /* Resource is owned, not borrowed. */
  content = XmDisclosureGetContent(section);
  header = XmDisclosureGetHeader(section);
  assert(content && header && XmIsRowColumn(content));
  assert(XmDisclosureGetContent(column) == NULL);
  field = XtVaCreateManagedWidget("field", xmTextFieldWidgetClass, content,
                                  XmNvalue, "Retained content", XmNcolumns, 30,
                                  NULL);
  hidden = XtVaCreateWidget("hidden", xmLabelWidgetClass, content, NULL);
  XtVaCreateManagedWidget("other", xmPushButtonWidgetClass, column, NULL);
  XtAddCallback(section, XmNexpandedCallback, changed, NULL);
  XtRealizeWidget(shell);
  pump(app, XtDisplay(shell));
  bridge = XmAccessibilityCreate(shell, "Disclosure Test");
  id = XmAccessibilityWidget(bridge, section);
  if (observe) {
    int attempts;
    XmAccessibilityConnect(bridge);
    for (attempts = 0; attempts < 100 && XmAccessibilityGetStatus(bridge) !=
                                             XmACCESSIBILITY_CONNECTED;
         attempts++)
      pump(app, XtDisplay(shell));
    assert(XmAccessibilityGetStatus(bridge) == XmACCESSIBILITY_CONNECTED);
    printf("{\"bus\":\"%s\",\"section\":%u}\n", XmAccessibilityBusName(bridge),
           id);
    fflush(stdout);
    XtAppMainLoop(app);
    return 0;
  }
  assert(id && XmAccessibilityQuery(bridge, id, &info));
  assert(info.states & XmACCESSIBLE_STATE(9));
  assert(info.states & XmACCESSIBLE_STATE(10));
  assert(!strcmp(info.name, "Section title") && info.action_count == 1);
  XmAccessibleInfoClear(&info);
  XtQueryGeometry(section, NULL, &geometry);
  expanded = geometry.height;
  assert(XmProcessTraversal(field, XmTRAVERSE_CURRENT));
  pump(app, XtDisplay(shell));
  assert(XmGetFocusWidget(shell) == field);
  XmDisclosureSetExpanded(section, False, True);
  pump(app, XtDisplay(shell));
  assert(notifications == 1 && !last_expanded);
  assert(XmGetFocusWidget(shell) == header);
  assert(XtIsManaged(content) && XtIsManaged(field) && !XtIsManaged(hidden));
  assert(
      XGetWindowAttributes(XtDisplay(content), XtWindow(content), &attributes));
  assert(attributes.map_state == IsUnmapped);
  XtQueryGeometry(section, NULL, &geometry);
  collapsed = geometry.height;
  {
    XtWidgetGeometry request, reply;
    Dimension before_width, after_width;
    XtVaGetValues(content, XmNwidth, &before_width, NULL);
    request.request_mode = CWWidth | XtCWQueryOnly;
    request.width = before_width + 17;
    assert(XtMakeGeometryRequest(content, &request, &reply) ==
           XtGeometryAlmost);
    XtVaGetValues(content, XmNwidth, &after_width, NULL);
    assert(before_width == after_width);
  }
  assert(collapsed < expanded);
  assert(XmAccessibilityQuery(bridge, id, &info));
  assert(!(info.states & XmACCESSIBLE_STATE(10)));
  XmAccessibleInfoClear(&info);
  XmDisclosureSetExpanded(section, False, True);
  assert(notifications == 1);
  XmDisclosureSetExpanded(section, True, False);
  pump(app, XtDisplay(shell));
  assert(notifications == 1 && XmDisclosureGetContent(section) == content);
  assert(
      XGetWindowAttributes(XtDisplay(content), XtWindow(content), &attributes));
  assert(attributes.map_state == IsViewable && !XtIsManaged(hidden));
  {
    char *value = XmTextFieldGetString(field);
    assert(!strcmp(value, "Retained content"));
    XtFree(value);
  }
  XtUnmanageChild(section);
  XtManageChild(section);
  pump(app, XtDisplay(shell));
  assert(XmDisclosureGetExpanded(section));
  assert(XmProcessTraversal(header, XmTRAVERSE_CURRENT));
  pump(app, XtDisplay(shell));
  key(header, XK_Left);
  pump(app, XtDisplay(shell));
  assert(!XmDisclosureGetExpanded(section) && notifications == 2);
  key(header, XK_Right);
  pump(app, XtDisplay(shell));
  assert(XmDisclosureGetExpanded(section) && notifications == 3);
  assert(XmAccessibilityAction(bridge, id, 0));
  pump(app, XtDisplay(shell));
  assert(!XmDisclosureGetExpanded(section) && notifications == 4);
  XtVaSetValues(section, XmNexpanded, True, NULL);
  pump(app, XtDisplay(shell));
  assert(notifications == 5 && last_expanded);
  key(header, XK_space);
  pump(app, XtDisplay(shell));
  assert(!XmDisclosureGetExpanded(section) && notifications == 6);
  key(header, XK_Return);
  pump(app, XtDisplay(shell));
  assert(XmDisclosureGetExpanded(section) && notifications == 7);
  {
    Widget rtl =
        XtVaCreateManagedWidget("rtl", xmDisclosureWidgetClass, column,
                                XmNlayoutDirection, XmRIGHT_TO_LEFT, NULL);
    Widget rtl_header = XmDisclosureGetHeader(rtl);
    XtAddCallback(rtl, XmNexpandedCallback, changed, NULL);
    pump(app, XtDisplay(shell));
    assert(XmProcessTraversal(rtl_header, XmTRAVERSE_CURRENT));
    pump(app, XtDisplay(shell));
    key(rtl_header, XK_Right);
    pump(app, XtDisplay(shell));
    assert(!XmDisclosureGetExpanded(rtl) && notifications == 8);
    key(rtl_header, XK_Left);
    pump(app, XtDisplay(shell));
    assert(XmDisclosureGetExpanded(rtl) && notifications == 9);
    XtDestroyWidget(rtl);
  }
  XtVaGetValues(section, XmNwidth, &width, NULL);
  XtVaGetValues(content, XmNwidth, &host_width, NULL);
  assert(width >= host_width && host_width > 0);
  XtVaSetValues(field, XmNcolumns, 50, NULL);
  pump(app, XtDisplay(shell));
  XtQueryGeometry(section, NULL, &geometry);
  assert(geometry.width >
         width); /* Child changes propagate a preferred size. */
  {
    XtCallbackRec close_callbacks[] = {{close_section, NULL}, {NULL, NULL}};
    Widget transient = XtVaCreateManagedWidget(
        "transient", xmDisclosureWidgetClass, column, XmNexpanded, False,
        XmNexpandedCallback, close_callbacks, NULL);
    assert(!destroyed_in_callback && !XmDisclosureGetExpanded(transient));
    XmAccessibilityRefresh(bridge);
    XmDisclosureSetExpanded(transient, True, True);
    assert(destroyed_in_callback);
    pump(app, XtDisplay(shell));
  }
  printf("disclosure: expanded=%u collapsed=%u callbacks=%d "
         "focus/retention/keyboard/semantics passed\n",
         expanded, collapsed, notifications);
  XmAccessibilityDestroy(bridge);
  XtDestroyWidget(shell);
  XtDestroyApplicationContext(app);
  return 0;
}
