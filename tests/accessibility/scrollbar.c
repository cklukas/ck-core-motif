/* Native scalar controls and custom Value provider fixture. LGPL-2.0-or-later.
 */
#include <X11/Shell.h>
#include <Xm/Accessible.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  XtAppContext app;
  Widget shell, bar, horizontal, monitor;
  XmAccessibility *bridge;
  XmAccessibleId custom;
  double scalar;
  unsigned callbacks, phase, checks;
} Fixture;
static Boolean scalar_query(XtPointer data, XmAccessibleInfo *i) {
  Fixture *f = data;
  i->role = XmACCESSIBLE_SCROLL_BAR;
  i->name = XtNewString("Continuous scalar");
  i->has_value = True;
  i->minimum_value = -1;
  i->maximum_value = 1;
  i->current_value = f->scalar;
  i->minimum_increment = .125;
  i->value_text = XtNewString("Scalar units");
  i->states = XmACCESSIBLE_STATE(8) | XmACCESSIBLE_STATE(7);
  return True;
}
static Boolean scalar_set(XtPointer data, double value) {
  Fixture *f = data;
  f->scalar = value;
  XmAccessibilityChanged(f->bridge, f->custom);
  return True;
}
static void changed(Widget widget, XtPointer data, XtPointer call) {
  Fixture *f = data;
  char text[128];
  XmString label;
  int value, size, increment, page;
  (void)call;
  XmScrollBarGetValues(widget, &value, &size, &increment, &page);
  snprintf(text, sizeof text, "callbacks %u value %d", ++f->callbacks, value);
  label = XmStringCreateLocalized(text);
  XtVaSetValues(f->monitor, XmNlabelString, label, NULL);
  XmStringFree(label);
}
static void control(Widget w, XtPointer data, XtPointer call) {
  Fixture *f = data;
  (void)w;
  (void)call;
  if (++f->phase == 1)
    XtVaSetValues(f->bar, XmNvalue, 40, XmNminimum, -10, XmNmaximum, 130,
                  XmNsliderSize, 30, XmNincrement, 5, NULL);
  else if (f->phase == 2)
    XtVaSetValues(f->bar, XmNeditable, False, NULL);
  else if (f->phase == 3) {
    XtDestroyWidget(f->bar);
    f->bar = NULL;
  } else {
    XmAccessibilityDestroy(f->bridge);
    f->bridge = NULL;
    /* Deferred transport teardown must leave native resources usable. */
    XmScrollBarSetValues(f->horizontal, 17, 0, 0, 0, True);
  }
}
static void finish(XtPointer data, XtIntervalId *id) {
  Fixture *f = data;
  (void)id;
  if (f->bridge)
    XmAccessibilityDestroy(f->bridge);
  XmScrollBarSetValues(f->horizontal, 19, 0, 0, 0, True);
  XtDestroyWidget(f->shell);
  puts("native-after-destroy-ok");
  fflush(stdout);
  exit(0);
}
static void connected(XtPointer data, XtIntervalId *id) {
  Fixture *f = data;
  (void)id;
  if (XmAccessibilityGetStatus(f->bridge) == XmACCESSIBILITY_CONNECTED) {
    printf("{\"bus\":\"%s\",\"bar\":%u,\"horizontal\":%u,\"custom\":%u,"
           "\"monitor\":%u,\"control\":%u,\"window\":%lu}\n",
           XmAccessibilityBusName(f->bridge),
           XmAccessibilityWidget(f->bridge, f->bar),
           XmAccessibilityWidget(f->bridge, f->horizontal), f->custom,
           XmAccessibilityWidget(f->bridge, f->monitor),
           XmAccessibilityWidget(f->bridge,
                                 XtNameToWidget(XtParent(f->bar), "control")),
           (unsigned long)XtWindow(f->shell));
    fflush(stdout);
    return;
  }
  if (++f->checks == 60)
    exit(2);
  XtAppAddTimeOut(f->app, 100, connected, f);
}
int main(int argc, char **argv) {
  Fixture f;
  Widget row, button;
  XmAccessibleId id;
  XmAccessibleInfo info;
  XmAccessibleProvider provider;
  memset(&f, 0, sizeof f);
  memset(&provider, 0, sizeof provider);
  f.shell = XtVaAppInitialize(&f.app, "ScalarFixture", NULL, 0, &argc, argv,
                              NULL, NULL);
  row = XtVaCreateManagedWidget("controls", xmRowColumnWidgetClass, f.shell,
                                NULL);
  f.monitor =
      XtVaCreateManagedWidget("monitor", xmPushButtonWidgetClass, row, NULL);
  f.bar = XtVaCreateManagedWidget(
      "Vertical position", xmScrollBarWidgetClass, row, XmNminimum, 0,
      XmNmaximum, 100, XmNsliderSize, 20, XmNvalue, 20, XmNincrement, 3,
      XmNpageIncrement, 20, XmNtraversalOn, True, NULL);
  f.horizontal = XtVaCreateManagedWidget(
      "Reverse horizontal position", xmScrollBarWidgetClass, row,
      XmNorientation, XmHORIZONTAL, XmNprocessingDirection, XmMAX_ON_LEFT,
      XmNminimum, 0, XmNmaximum, 100, XmNsliderSize, 10, XmNvalue, 20,
      XmNincrement, 2, XmNtraversalOn, True, NULL);
  button =
      XtVaCreateManagedWidget("control", xmPushButtonWidgetClass, row, NULL);
  XtAddCallback(button, XmNactivateCallback, control, &f);
  XtRealizeWidget(f.shell);
  f.bridge = XmAccessibilityCreate(f.shell, "Scalar controls fixture");
  id = XmAccessibilityWidget(f.bridge, f.bar);
  XtAddCallback(f.bar, XmNvalueChangedCallback, changed, &f);
  assert(XmAccessibilityQuery(f.bridge, id, &info));
  assert(info.role == XmACCESSIBLE_SCROLL_BAR && info.has_value &&
         info.maximum_value == 80);
  XmAccessibleInfoClear(&info);
  assert(!XmAccessibilitySetValue(f.bridge, id, NAN));
  assert(!XmAccessibilitySetValue(f.bridge, id, INFINITY));
  assert(!XmAccessibilitySetValue(f.bridge, id, 20.5));
  assert(!XmAccessibilitySetValue(f.bridge, id, 81));
  assert(!XmAccessibilitySetValue(f.bridge, id, -1));
  assert(XmAccessibilitySetValue(f.bridge, id, 23) && f.callbacks == 1);
  assert(XmAccessibilityAction(f.bridge, id, 1) && f.callbacks == 2);
  assert(!XmAccessibilityAction(f.bridge, id, 6));
  XtSetSensitive(f.bar, False);
  assert(!XmAccessibilitySetValue(f.bridge, id, 23));
  assert(!XmAccessibilityAction(f.bridge, id, 0));
  XtSetSensitive(f.bar, True);
  provider.query = scalar_query;
  provider.set_value = scalar_set;
  f.custom = XmAccessibilityAdd(f.bridge, XmAccessibilityWidget(f.bridge, row),
                                &provider, &f);
  assert(XmAccessibilitySetValue(f.bridge, f.custom, .25));
  assert(!XmAccessibilitySetValue(f.bridge, f.custom, 2));
  f.scalar = NAN;
  assert(!XmAccessibilityQuery(f.bridge, f.custom, &info));
  f.scalar = .25;
  XmAccessibilityConnect(f.bridge);
  XtAppAddTimeOut(f.app, 100, connected, &f);
  XtAppAddTimeOut(f.app, 12000, finish, &f);
  XtAppMainLoop(f.app);
  return 0;
}
