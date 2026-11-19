/* Automated regression fixture for session-wide XM_ACCESSIBILITY=1 AT-SPI2
 * auto-bootstrap without explicit XmAccessibilityCreate() in application main.
 * LGPL-2.0-or-later.
 */
#include <X11/Shell.h>
#include <Xm/Accessible.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  XtAppContext app;
  Widget shell;
  Widget second_shell;
  Widget status_label;
  Widget action_btn;
  Widget toggle_btn;
  Widget quit_btn;
  Widget text_field;
  Widget multi_text;
  Widget list_box;
  int poll_ticks;
} AutoFixture;

static void set_label_string(Widget w, const char *s) {
  XmString xs = XmStringCreateLocalized((String)s);
  XtVaSetValues(w, XmNlabelString, xs, NULL);
  XmStringFree(xs);
  XmAccessibility *a = XmAccessibilityGetDefault(w);
  if (a) XmAccessibilityChanged(a, XmAccessibilityWidget(a, w));
}

static void on_action_btn(Widget w, XtPointer data, XtPointer call) {
  AutoFixture *f = (AutoFixture *)data;
  (void)w;
  (void)call;
  set_label_string(f->status_label, "Status: Button Invoked");
}

static void on_toggle_btn(Widget w, XtPointer data, XtPointer call) {
  AutoFixture *f = (AutoFixture *)data;
  XmToggleButtonCallbackStruct *cbs = (XmToggleButtonCallbackStruct *)call;
  (void)w;
  set_label_string(f->status_label,
                   (cbs && cbs->set) ? "Status: Toggle On" : "Status: Toggle Off");
  XmAccessibility *a = XmAccessibilityGetDefault(f->toggle_btn);
  if (a) XmAccessibilityChanged(a, XmAccessibilityWidget(a, f->toggle_btn));
}

static void deferred_quit(XtPointer data, XtIntervalId *id) {
  AutoFixture *f = (AutoFixture *)data;
  (void)id;
  if (f->second_shell) {
    XtDestroyWidget(f->second_shell);
    f->second_shell = NULL;
  }
  if (f->shell) {
    XtDestroyWidget(f->shell);
    f->shell = NULL;
  }
  XtAppSetExitFlag(f->app);
}

static void on_quit_btn(Widget w, XtPointer data, XtPointer call) {
  AutoFixture *f = (AutoFixture *)data;
  (void)w;
  (void)call;
  XtAppAddTimeOut(f->app, 20, deferred_quit, f);
}

static void timeout_abort(XtPointer data, XtIntervalId *id) {
  (void)data;
  (void)id;
  fprintf(stderr, "check-auto-atspi-bridge timed out\n");
  exit(2);
}

static void poll_connected(XtPointer data, XtIntervalId *id) {
  AutoFixture *f = (AutoFixture *)data;
  XmAccessibility *a = XmAccessibilityGetDefault(f->shell);
  (void)id;
  if (!a) {
    fprintf(stderr, "XmAccessibilityGetDefault returned NULL when expected active\n");
    exit(3);
  }
  XmAccessibilityStatus st = XmAccessibilityGetStatus(a);
  if (st == XmACCESSIBILITY_CONNECTED) {
    printf("{\"pid\":%d,\"bus\":\"%s\"}\n", (int)getpid(), XmAccessibilityBusName(a));
    fflush(stdout);
    return;
  }
  if (++f->poll_ticks > 80 || st == XmACCESSIBILITY_NOT_BUILT ||
      st == XmACCESSIBILITY_UNAVAILABLE || st == XmACCESSIBILITY_DISCONNECTED) {
    fprintf(stderr, "auto bridge failed to connect (status=%d)\n", (int)st);
    exit(4);
  }
  XtAppAddTimeOut(f->app, 50, poll_connected, f);
}

int main(int argc, char **argv) {
  AutoFixture f;
  int verify_disabled = 0;
  Widget rc;
  XmString items[3];
  int i;

  memset(&f, 0, sizeof(f));
  for (i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "--verify-disabled")) {
      verify_disabled = 1;
    }
  }

  setlocale(LC_ALL, "");
  XtSetLanguageProc(NULL, NULL, NULL);
  f.shell = XtVaAppInitialize(&f.app, "AutoAtspiFixture", NULL, 0, &argc, argv,
                              NULL, XmNtitle, "Auto AT-SPI Window", NULL);

  rc = XtVaCreateManagedWidget("mainColumn", xmRowColumnWidgetClass, f.shell,
                               XmNorientation, XmVERTICAL, NULL);

  {
    XmString lbl = XmStringCreateLocalized("Status: Ready");
    f.status_label = XtVaCreateManagedWidget("statusLabel", xmLabelWidgetClass,
                                             rc, XmNlabelString, lbl, NULL);
    XmStringFree(lbl);
  }
  {
    XmString lbl = XmStringCreateLocalized("Invoke Action");
    f.action_btn = XtVaCreateManagedWidget("invokeBtn", xmPushButtonWidgetClass,
                                           rc, XmNlabelString, lbl, NULL);
    XmStringFree(lbl);
    XtAddCallback(f.action_btn, XmNactivateCallback, on_action_btn, &f);
  }
  {
    XmString lbl = XmStringCreateLocalized("Enable Option");
    f.toggle_btn = XtVaCreateManagedWidget("enableToggle", xmToggleButtonWidgetClass,
                                           rc, XmNlabelString, lbl, NULL);
    XmStringFree(lbl);
    XtAddCallback(f.toggle_btn, XmNvalueChangedCallback, on_toggle_btn, &f);
  }

  f.text_field = XtVaCreateManagedWidget("singleField", xmTextFieldWidgetClass,
                                         rc, XmNvalue, "Hello Field", NULL);
  f.multi_text = XtVaCreateManagedWidget("multiText", xmTextWidgetClass, rc,
                                         XmNeditMode, XmMULTI_LINE_EDIT,
                                         XmNrows, 2, XmNcolumns, 20,
                                         XmNvalue, "Multi-line\nMotif text", NULL);

  items[0] = XmStringCreateLocalized("Alpha");
  items[1] = XmStringCreateLocalized("Beta");
  items[2] = XmStringCreateLocalized("Gamma");
  f.list_box = XtVaCreateManagedWidget("sampleList", xmListWidgetClass, rc,
                                       XmNitems, items, XmNitemCount, 3,
                                       XmNvisibleItemCount, 3, NULL);
  XmStringFree(items[0]);
  XmStringFree(items[1]);
  XmStringFree(items[2]);

  {
    XmString lbl = XmStringCreateLocalized("Quit Fixture");
    f.quit_btn = XtVaCreateManagedWidget("quitBtn", xmPushButtonWidgetClass,
                                         rc, XmNlabelString, lbl, NULL);
    XmStringFree(lbl);
    XtAddCallback(f.quit_btn, XmNactivateCallback, on_quit_btn, &f);
  }

  XtRealizeWidget(f.shell);

  if (verify_disabled) {
    if (XmAccessibilityGetDefault(f.shell) != NULL) {
      fprintf(stderr, "Expected XmAccessibilityGetDefault == NULL when disabled\n");
      return 1;
    }
    XtDestroyWidget(f.shell);
    puts("{\"disabled_verified\":true}");
    return 0;
  }

  XSetInputFocus(XtDisplay(f.shell), XtWindow(f.shell), RevertToParent,
                 CurrentTime);
  XSync(XtDisplay(f.shell), False);

  f.second_shell = XtVaAppCreateShell("secondaryShell", "AutoAtspiFixture",
                                      topLevelShellWidgetClass,
                                      XtDisplay(f.shell),
                                      XmNtitle, "Secondary Dialog", NULL);
  {
    XmString lbl = XmStringCreateLocalized("Secondary Button");
    XtVaCreateManagedWidget("secondaryBtn", xmPushButtonWidgetClass,
                            f.second_shell, XmNlabelString, lbl, NULL);
    XmStringFree(lbl);
  }
  XtRealizeWidget(f.second_shell);

  XtAppAddTimeOut(f.app, 50, poll_connected, &f);
  XtAppAddTimeOut(f.app, 15000, timeout_abort, &f);
  XtAppMainLoop(f.app);
  return 0;
}
