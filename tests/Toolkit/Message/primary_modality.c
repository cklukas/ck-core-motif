/* Native input regression for primary-window and full-application modality.
 * Run only on an explicitly isolated X server: the fixture moves input focus.
 */
#include <X11/Shell.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <Xm/DisplayP.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScreenP.h>
#include <Xm/TextF.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Internal grab entry points are exercised against the installed library;
 * no private implementation source is linked into this consumer. */
#if NeedWidePrototypes
extern void _XmAddGrab(Widget, int, int);
#else
extern void _XmAddGrab(Widget, Boolean, Boolean);
#endif
extern void _XmRemoveGrab(Widget);

typedef struct {
  Widget shell, body, button, text;
  unsigned clicks;
} Pane;
static XtAppContext context;
static Display *display;
static unsigned errors, warnings, assertions;

static void require(int condition, const char *detail) {
  ++assertions;
  if (!condition) {
    fprintf(stderr, "FAIL: %s\n", detail);
    exit(1);
  }
}
static int x_error(Display *d, XErrorEvent *event) {
  (void)d;
  ++errors;
  fprintf(stderr, "X error %u request %u\n", event->error_code,
          event->request_code);
  return 0;
}
static void retired(Widget widget, XtPointer data, XtPointer call) {
  (void)widget;
  (void)call;
  *(Boolean *)data = True;
}
static void warning(String message) {
  ++warnings;
  fprintf(stderr, "Xt warning: %s\n", message);
}
static void settle(void) {
  unsigned cycle;
  const struct timespec delay = {0, 2000000};
  for (cycle = 0; cycle < 60; ++cycle) {
    unsigned budget = 1000;
    while (XtAppPending(context) && budget--)
      XtAppProcessEvent(context, XtIMAll);
    XSync(display, False);
    nanosleep(&delay, NULL);
  }
}
static void clicked(Widget widget, XtPointer data, XtPointer call) {
  (void)widget;
  (void)call;
  ++((Pane *)data)->clicks;
}
static void contents(Pane *pane) {
  Widget row = XtVaCreateManagedWidget("content", xmRowColumnWidgetClass,
                                       pane->body, NULL);
  pane->text = XtVaCreateManagedWidget("entry", xmTextFieldWidgetClass, row,
                                       XmNcolumns, 12, NULL);
  pane->button =
      XtVaCreateManagedWidget("activate", xmPushButtonWidgetClass, row, NULL);
  XtAddCallback(pane->button, XmNactivateCallback, clicked, pane);
}
static void top(Pane *pane, const char *name, int x) {
  memset(pane, 0, sizeof(*pane));
  pane->shell = XtVaAppCreateShell(
      name, "ModalFixture", applicationShellWidgetClass, display, XmNx, x, XmNy,
      20, XmNwidth, 200, XmNheight, 130, NULL);
  pane->body =
      XtVaCreateManagedWidget("body", xmFormWidgetClass, pane->shell, NULL);
  contents(pane);
  XtRealizeWidget(pane->shell);
  settle();
}
typedef struct {
  Pane panes[24];
  int ran;
} ReentrantWindows;
static void create_windows_on_popdown(Widget widget, XtPointer data,
                                      XtPointer call) {
  ReentrantWindows *windows = (ReentrantWindows *)data;
  unsigned i;
  (void)widget;
  (void)call;
  if (windows->ran)
    return;
  windows->ran = 1;
  for (i = 0; i < 24; ++i)
    top(&windows->panes[i], "reentrant-top", 10);
}
static void dialog(Pane *pane, Widget parent, const char *name,
                   unsigned char style) {
  Arg args[2];
  memset(pane, 0, sizeof(*pane));
  XtSetArg(args[0], XmNdialogStyle, style);
  XtSetArg(args[1], XmNautoUnmanage, False);
  pane->body = XmCreateFormDialog(parent, (char *)name, args, 2);
  pane->shell = XtParent(pane->body);
  contents(pane);
  XtManageChild(pane->body);
  settle();
}
static void input(Pane *pane, int permitted, const char *phase) {
  Dimension width, height;
  Window child;
  int x, y;
  unsigned previous = pane->clicks;
  char *text;
  KeyCode key = XKeysymToKeycode(display, XK_x);
  require(key != 0, "X keyboard has x key");
  XRaiseWindow(display, XtWindow(pane->shell));
  settle();
  XtVaGetValues(pane->button, XmNwidth, &width, XmNheight, &height, NULL);
  require(XTranslateCoordinates(display, XtWindow(pane->button),
                                DefaultRootWindow(display), width / 2,
                                height / 2, &x, &y, &child),
          "actual button position");
  require(XTestFakeMotionEvent(display, DefaultScreen(display), x, y, 0),
          "native pointer move");
  require(XTestFakeButtonEvent(display, 1, True, 0) &&
              XTestFakeButtonEvent(display, 1, False, 0),
          "native button input");
  settle();
  if (pane->clicks != previous + (permitted ? 1U : 0U)) {
    fprintf(stderr, "phase %s pointer: expected %d got %u\n", phase, permitted,
            pane->clicks - previous);
    require(0, "modal pointer scope");
  }
  XmTextFieldSetString(pane->text, "");
  XtSetKeyboardFocus(pane->shell, pane->text);
  XSetInputFocus(display, XtWindow(pane->text), RevertToParent, CurrentTime);
  require(XTestFakeKeyEvent(display, key, True, 0) &&
              XTestFakeKeyEvent(display, key, False, 0),
          "native keyboard input");
  settle();
  text = XmTextFieldGetString(pane->text);
  if (!text || strcmp(text, permitted ? "x" : "")) {
    fprintf(stderr, "phase %s keyboard: expected %d got '%s'\n", phase,
            permitted, text ? text : "<null>");
    XtFree(text);
    require(0, "modal keyboard scope");
  }
  XtFree(text);
  require(errors == 0 && warnings == 0, "input has no X/Xt diagnostics");
}
static void popdown(Pane *pane) {
  XtUnmanageChild(pane->body);
  settle();
}
static void destroy(Pane *pane) {
  XtDestroyWidget(pane->shell);
  pane->shell = pane->body = pane->text = pane->button = NULL;
  settle();
}
int main(int argc, char **argv) {
  Pane a, b, c, d, e, primary, nested, full, other, blocked;
  XmDisplay xm_display;
  Cardinal baseline, initial;
  Boolean display_retired = False;
  int event, error, major, minor;
  unsigned cycle;
  ReentrantWindows reentrant = {0};
  require(argc == 2 && !strcmp(argv[1], "--isolated-display"),
          "explicit isolated-display acknowledgement required");
  argc = 1;
  XtToolkitInitialize();
  context = XtCreateApplicationContext();
  XtAppSetWarningHandler(context, warning);
  display = XtOpenDisplay(context, NULL, "modality", "ModalFixture", NULL, 0,
                          &argc, argv);
  require(display != NULL, "isolated X display");
  XSetErrorHandler(x_error);
  require(XTestQueryExtension(display, &event, &error, &major, &minor),
          "XTEST extension available");
  require(!((XmScreen)XmGetXmScreen(DefaultScreenOfDisplay(display)))
               ->screen.mwmPresent,
          "non-Motif window-manager path required");
  xm_display = (XmDisplay)XmGetXmDisplay(display);
  XtAddCallback((Widget)xm_display, XmNdestroyCallback, retired,
                &display_retired);
  initial = xm_display->display.numModals;
  top(&a, "first", 10);
  top(&b, "second", 240);
  baseline = xm_display->display.numModals;
  input(&a, 1, "baseline-first");
  input(&b, 1, "baseline-second");

  /* A real MenuShell popdown callback reallocates the live modal array while
   * the incoming primary dialog is still establishing its grab. */
  {
    Widget popup = XmCreatePopupMenu(b.body, "reentrant-popup", NULL, 0);
    XButtonPressedEvent press;
    XWindowAttributes attributes;
    XtVaCreateManagedWidget("menu-item", xmPushButtonWidgetClass, popup, NULL);
    XtAddCallback(XtParent(popup), XmNpopdownCallback,
                  create_windows_on_popdown, &reentrant);
    memset(&press, 0, sizeof(press));
    press.type = ButtonPress;
    press.display = display;
    press.window = XtWindow(b.body);
    press.root = DefaultRootWindow(display);
    press.x_root = 400;
    press.y_root = 400;
    press.button = Button3;
    press.same_screen = True;
    press.time = XtLastTimestampProcessed(display);
    XmMenuPosition(popup, &press);
    XtManageChild(popup);
    settle();
    require(
        XGetWindowAttributes(display, XtWindow(XtParent(popup)), &attributes) &&
            attributes.map_state == IsViewable,
        "actual popup menu posted before modal transition");
    dialog(&primary, a.body, "reentrant-primary",
           XmDIALOG_PRIMARY_APPLICATION_MODAL);
    require(reentrant.ran,
            "modal transition invoked actual menu popdown callback");
    input(&a, 0, "reentrant-popup-parent-blocked");
    input(&b, 1, "reentrant-popup-independent-allowed");
    input(&reentrant.panes[0], 1, "reentrant-new-window-allowed");
    input(&primary, 1, "reentrant-popup-modal-allowed");
    destroy(&primary);
    XtDestroyWidget(XtParent(popup));
    settle();
    for (cycle = 0; cycle < 24; ++cycle)
      destroy(&reentrant.panes[cycle]);
    require(xm_display->display.numModals == baseline,
            "reentrant modal transition retains exact baseline grab count");
  }
  {
    Widget disposable = XtVaCreateManagedWidget(
        "disposable-raw", xmPushButtonWidgetClass, b.body, NULL);
    _XmAddGrab(disposable, False, False);
    _XmAddGrab(disposable, False, False);
    XtDestroyWidget(disposable);
    settle();
    require(xm_display->display.numModals == baseline,
            "duplicate non-VendorShell destruction removes exact raw grabs");
    input(&a, 1, "raw-destroy-restores-first");
    input(&b, 1, "raw-destroy-restores-second");
  }

  dialog(&primary, a.body, "primary", XmDIALOG_PRIMARY_APPLICATION_MODAL);
  input(&a, 0, "primary-parent-blocked");
  input(&b, 1, "primary-independent-allowed");
  input(&primary, 1, "primary-dialog-allowed");
  _XmAddGrab(a.button, False, False);
  input(&a, 0, "raw-grab-cannot-reopen-blocked-parent");
  input(&b, 1, "raw-blocked-grab-retains-independent-branch");
  _XmRemoveGrab(a.button);
  _XmAddGrab(a.shell, False, False);
  input(&a, 0, "raw-shell-grab-cannot-reopen-parent");
  _XmRemoveGrab(a.shell);
  input(&b, 1, "raw-shell-removal-preserves-earlier-vendor-grab");
  input(&primary, 1, "raw-shell-removal-preserves-primary-dialog");
  _XmAddGrab(primary.button, False, False);
  input(&primary, 1, "raw-grab-in-modal-descendant-allowed");
  _XmRemoveGrab(primary.button);
  _XmAddGrab(primary.body, True, True);
  input(&b, 0, "raw-exclusive-scope-blocks-independent");
  input(&primary, 1, "raw-exclusive-descendant-allowed");
  _XmRemoveGrab(primary.body);
  input(&b, 1, "raw-exclusive-pop-restores-primary-cousin");
  top(&c, "late-primary", 470);
  input(&c, 1, "primary-new-independent-allowed");
  dialog(&nested, primary.body, "nested", XmDIALOG_PRIMARY_APPLICATION_MODAL);
  input(&a, 0, "nested-ancestor-blocked");
  input(&primary, 0, "nested-parent-blocked");
  input(&b, 1, "nested-independent-allowed");
  input(&nested, 1, "nested-dialog-allowed");
  popdown(&nested);
  input(&primary, 1, "nested-popdown-restores-parent-dialog");
  input(&a, 0, "nested-popdown-retains-primary");
  XtManageChild(nested.body);
  settle();
  destroy(&nested);
  input(&primary, 1, "nested-destroy-restores-parent-dialog");

  dialog(&full, primary.body, "full", XmDIALOG_FULL_APPLICATION_MODAL);
  input(&a, 0, "full-ancestor-blocked");
  input(&b, 0, "full-independent-blocked");
  input(&primary, 0, "full-parent-dialog-blocked");
  input(&full, 1, "full-dialog-allowed");
  dialog(&blocked, b.body, "blocked-primary",
         XmDIALOG_PRIMARY_APPLICATION_MODAL);
  input(&blocked, 0, "new-unrelated-primary-cannot-escape-full-modal");
  input(&full, 1, "new-unrelated-primary-retains-original-full-modal");
  popdown(&full);
  input(&blocked, 1, "full-pop-admits-deferred-primary");
  input(&b, 0, "deferred-primary-now-blocks-its-parent");
  input(&primary, 1, "deferred-primary-retains-independent-dialog");
  destroy(&blocked);
  input(&b, 1, "deferred-primary-destroy-restores-parent");
  XtManageChild(full.body);
  settle();
  input(&b, 0, "reposted-full-blocks-independent");
  input(&full, 1, "reposted-full-dialog-allowed");
  top(&d, "late-full", 700);
  input(&d, 0, "full-new-independent-blocked");
  dialog(&nested, full.body, "inside-full", XmDIALOG_PRIMARY_APPLICATION_MODAL);
  input(&full, 0, "primary-in-full-parent-blocked");
  input(&b, 0, "primary-in-full-retains-outer-scope");
  input(&nested, 1, "primary-in-full-dialog-allowed");
  top(&e, "late-nested-full", 800);
  input(&e, 0, "primary-in-full-new-independent-blocked");
  destroy(&nested);
  input(&full, 1, "nested-pop-retains-full");
  input(&e, 0, "nested-pop-retains-full-new-window");
  popdown(&full);
  input(&primary, 1, "full-pop-restores-primary");
  input(&a, 0, "full-pop-retains-primary-parent-block");
  input(&b, 1, "full-pop-restores-independent");
  input(&d, 1, "full-pop-admits-late-window");
  input(&e, 1, "full-pop-admits-nested-late-window");
  destroy(&full);

  dialog(&other, b.body, "other-primary", XmDIALOG_PRIMARY_APPLICATION_MODAL);
  input(&a, 0, "parallel-primary-first-parent-blocked");
  input(&b, 0, "parallel-primary-second-parent-blocked");
  input(&primary, 1, "parallel-primary-first-dialog-allowed");
  input(&other, 1, "parallel-primary-second-dialog-allowed");
  popdown(&primary);
  input(&a, 1, "earlier-primary-pop-restores-independent-branch");
  input(&b, 0, "earlier-primary-pop-retains-other-parent-block");
  destroy(&primary);
  popdown(&other);
  input(&b, 1, "last-primary-pop-restores-parent");
  destroy(&other);

  destroy(&c);
  destroy(&d);
  destroy(&e);
  require(xm_display->display.numModals == baseline,
          "modal teardown preserves exact baseline grab count");
  for (cycle = 0; cycle < 12; ++cycle) {
    dialog(&primary, a.body, "repeated-primary",
           XmDIALOG_PRIMARY_APPLICATION_MODAL);
    dialog(&nested, primary.body, "repeated-nested",
           XmDIALOG_PRIMARY_APPLICATION_MODAL);
    destroy(&primary); /* Destruction also removes its nested dialog. */
    require(xm_display->display.numModals == baseline,
            "nested destroy does not retain virtual grab entries");
  }
  input(&a, 1, "repeated-destroy-restores-first");
  input(&b, 1, "repeated-destroy-restores-second");
  dialog(&primary, a.body, "parent-destroy",
         XmDIALOG_PRIMARY_APPLICATION_MODAL);
  dialog(&nested, primary.body, "nested-parent-destroy",
         XmDIALOG_PRIMARY_APPLICATION_MODAL);
  destroy(&a);
  input(&b, 1, "destroyed-primary-family-does-not-block-independent");
  require(!display_retired, "display remains alive for the final window");
  require(xm_display->display.numModals == initial + 1,
          "only the final top-level grab remains");
  destroy(&b);
  /* The last VendorShell can retire XmDisplay itself. The destroy callback
   * records that lifetime boundary before any possible post-destroy access. */
  require(display_retired || xm_display->display.numModals == initial,
          "all owned grabs removed or their display retired");
  if (!display_retired)
    XtRemoveCallback((Widget)xm_display, XmNdestroyCallback, retired,
                     &display_retired);
  require(errors == 0 && warnings == 0, "no X/Xt errors during cleanup");
  XtCloseDisplay(display);
  XtDestroyApplicationContext(context);
  printf("primary/full modality, nested and parallel dialogs, new windows, "
         "actual pointer/keyboard input and teardown passed (%u assertions)\n",
         assertions);
  return 0;
}
