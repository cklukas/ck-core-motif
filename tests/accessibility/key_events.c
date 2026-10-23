/* Real native keys must reach optional toolkit listeners before widget edits. */
#include <Xm/Accessible.h>
#include <Xm/TextF.h>
#include <X11/Shell.h>
#include <X11/keysym.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
typedef struct {
  XtAppContext app;
  Widget shell, second, field, other;
  XmAccessibility *bridge;
  XmAccessibleId field_id, other_id, control;
  unsigned int attempts, marks;
} Fixture;
static Fixture *current;
static void mark(Widget w, XEvent *event, String *params, Cardinal *count) {
  (void)w; (void)event; (void)params; (void)count;
  ++current->marks;
}
static Boolean control_query(XtPointer data, XmAccessibleInfo *i) {
  (void)data;
  i->role = XmACCESSIBLE_BUTTON; i->name = XtNewString("Lifecycle control");
  i->action_count = 2; return True;
}
static Boolean control_action(XtPointer data, unsigned int action) {
  Fixture *f = data;
  if (action == 0) {
    XmAccessibility *bridge = f->bridge;
    f->bridge = NULL; XmAccessibilityDestroy(bridge); return True;
  }
  if (action == 1) {
    XEvent key = {0};
    key.xkey.type = KeyPress; key.xkey.display = XtDisplay(f->field);
    key.xkey.window = XtWindow(f->field); key.xkey.root = DefaultRootWindow(key.xkey.display);
    key.xkey.keycode = XKeysymToKeycode(key.xkey.display, XK_n); key.xkey.same_screen = True;
    XtDispatchEventToWidget(f->field, &key); return True;
  }
  return False;
}
static void command(XtPointer data, int *fd, XtInputId *input) {
  Fixture *f = data; char line[32]; char *value, *other;
  (void)fd; (void)input;
  if (!fgets(line, sizeof(line), stdin) || !strncmp(line, "quit", 4)) {
    if (f->bridge) XmAccessibilityDestroy(f->bridge);
    if (f->second) XtDestroyWidget(f->second);
    XtDestroyWidget(f->shell); exit(0);
  }
  if (!strncmp(line, "focus1", 6)) XmProcessTraversal(f->field, XmTRAVERSE_CURRENT);
  else if (!strncmp(line, "focus2", 6)) XmProcessTraversal(f->other, XmTRAVERSE_CURRENT);
  else if (!strncmp(line, "close2", 6)) {
    XtDestroyWidget(f->second); f->second = NULL; f->other = NULL;
  } else if (strncmp(line, "state", 5)) exit(4);
  value = XmTextFieldGetString(f->field);
  other = f->other ? XmTextFieldGetString(f->other) : XtNewString("");
  /* Observer sends only known ASCII fixture keys. */
  printf("{\"value\":\"%s\",\"other\":\"%s\",\"marks\":%u,\"bridge\":%s}\n",
      value, other, f->marks, f->bridge ? "true" : "false");
  XtFree(value); XtFree(other); fflush(stdout);
}
static void connected(XtPointer data, XtIntervalId *timer) {
  Fixture *f = data; (void)timer;
  if (XmAccessibilityGetStatus(f->bridge) == XmACCESSIBILITY_CONNECTED) {
    printf("{\"bus\":\"%s\",\"window\":%lu,\"second\":%lu,\"field\":%u,\"other\":%u,\"control\":%u}\n",
      XmAccessibilityBusName(f->bridge), XtWindow(f->shell), XtWindow(f->second),
      f->field_id, f->other_id, f->control);
    fflush(stdout);
    XtAppAddInput(f->app, STDIN_FILENO, (XtPointer)XtInputReadMask, command, f);
  } else if (++f->attempts < 100) XtAppAddTimeOut(f->app, 100, connected, f);
  else exit(2);
}
int main(int argc, char **argv) {
  Fixture f = {0}; XmAccessibleProvider p = {0};
  XtActionsRec actions[] = {{"key-event-mark", mark}};
  if (!setlocale(LC_ALL, "")) return 5;
  XtSetLanguageProc(NULL, NULL, NULL);
  f.shell = XtVaAppInitialize(&f.app, "NativeKeyEvents", NULL, 0, &argc, argv, NULL, NULL);
  f.field = XtVaCreateManagedWidget("field", xmTextFieldWidgetClass, f.shell, NULL);
  f.second = XtVaAppCreateShell("second", "NativeKeyEvents", topLevelShellWidgetClass,
      XtDisplay(f.shell), NULL);
  f.other = XtVaCreateManagedWidget("other", xmTextFieldWidgetClass, f.second, NULL);
  XtAppAddActions(f.app, actions, XtNumber(actions)); current = &f;
  /* F10 is the native osfMenuBar virtual key. Use one ordinary binding in
   * observe/consume/observe modes to prove exact native action suppression. */
  XtOverrideTranslations(f.field, XtParseTranslationTable("<Key>F9: key-event-mark()"));
  XtRealizeWidget(f.shell); XtRealizeWidget(f.second);
  XtSetKeyboardFocus(f.shell, f.field); XtSetKeyboardFocus(f.second, f.other);
  f.bridge = XmAccessibilityCreate(f.shell, "Native key fixture");
  if (!f.bridge || !XmAccessibilityAttachShell(f.bridge, f.second)) return 1;
  f.field_id = XmAccessibilityWidget(f.bridge, f.field); f.other_id = XmAccessibilityWidget(f.bridge, f.other);
  p.query = control_query; p.action = control_action;
  f.control = XmAccessibilityAdd(f.bridge, f.field_id, &p, &f);
  XmAccessibilityConnect(f.bridge); XtAppAddTimeOut(f.app, 100, connected, &f);
  XtAppMainLoop(f.app); return 1;
}
