/* Actual X focus ownership and authoritative wrapped Text line boundaries. */
#include "../../lib/Xm/AccessibleI.h"
#include <Xm/TextP.h>
#include <Xm/TextF.h>
#include <X11/Shell.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
static void require(int value, const char *message) {
  if (!value) { fprintf(stderr, "%s\n", message); exit(1); }
}
static void drain(XtAppContext app, Display *display) {
  XSync(display, False);
  while (XtAppPending(app)) XtAppProcessEvent(app, XtIMAll);
}
static void active(XmAccessibility *bridge, Widget first, Widget second,
                   Widget dialog, Widget focus, Widget expected, XtAppContext app) {
  Widget shells[] = {first, second, dialog};
  size_t i;
  Display *display = XtDisplay(first);
  XSetInputFocus(display, XtWindow(focus), RevertToParent, CurrentTime);
  drain(app, display);
  for (i = 0; i < XtNumber(shells); ++i) {
    XmAccessibleInfo info;
    XmAccessibleId id = XmAccessibilityWidget(bridge, shells[i]);
    require(id && XmAccessibilityQuery(bridge, id, &info), "shell must remain queryable");
    require(!!(info.states & XmACCESSIBLE_STATE(1)) == (shells[i] == expected),
            "ACTIVE does not match actual owning X input-focus WM shell");
    XmAccessibleInfoClear(&info);
  }
}
static int lines(XmAccessibility *bridge, Widget widget) {
  XmTextWidget text = (XmTextWidget)widget;
  XmAccessibleId id = XmAccessibilityWidget(bridge, widget);
  int offset, direction, length = (int)XmTextGetLastPosition(widget);
  require(text->text.total_lines > 2, "fixture did not actually wrap text");
  for (offset = 0; offset <= length; ++offset) {
    int line = 0;
    while (line + 1 < text->text.total_lines &&
           text->text.line_table[line + 1].start_pos <= (unsigned int)offset) ++line;
    for (direction = -1; direction <= 1; ++direction) {
      int first = -1, last = -1, index = line + direction;
      int expected_first = index < 0 ? 0 : index >= text->text.total_lines ? length :
          (int)text->text.line_table[index].start_pos;
      int expected_last = index < 0 ? 0 : index + 1 >= text->text.total_lines ? length :
          (int)text->text.line_table[index + 1].start_pos;
      require(XmAccessibilityVisualLine(bridge, id, offset, direction, &first, &last), "native visual line unavailable");
      require(first == expected_first && last == expected_last, "visual line differs from actual wrapped layout");
    }
  }
  return text->text.total_lines;
}
int main(int argc, char **argv) {
  XtAppContext app;
  Widget shell, second, dialog, menu, text, field;
  XmAccessibility *bridge;
  int narrow, wide;
  require(setlocale(LC_ALL, "") != NULL, "locale");
  XtSetLanguageProc(NULL, NULL, NULL);
  shell = XtVaAppInitialize(&app, "NativeGeometry", NULL, 0, &argc, argv, NULL, NULL);
  text = XtVaCreateManagedWidget("wrapped", xmTextWidgetClass, shell,
      XmNeditMode, XmMULTI_LINE_EDIT, XmNwordWrap, True, XmNscrollHorizontal, False,
      XmNwidth, 150, XmNheight, 120, NULL);
  XmTextSetString(text, "Alpha beta gamma delta epsilon zeta eta theta iota kappa lambda mu.\nSecond line.\n");
  second = XtVaAppCreateShell("second", "NativeGeometry", topLevelShellWidgetClass,
                             XtDisplay(shell), NULL);
  field = XtVaCreateManagedWidget("field", xmTextFieldWidgetClass, second, NULL);
  dialog = XtVaCreatePopupShell("dialog", transientShellWidgetClass, shell,
                                XtNwidth, 100, XtNheight, 60, NULL);
  menu = XtVaCreatePopupShell("menu", overrideShellWidgetClass, shell,
                              XtNwidth, 80, XtNheight, 40, NULL);
  XtRealizeWidget(shell); XtRealizeWidget(second);
  XtPopup(dialog, XtGrabNone); XtPopup(menu, XtGrabNone);
  drain(app, XtDisplay(shell));
  bridge = XmAccessibilityCreate(shell, "Native geometry");
  require(bridge != NULL, "bridge");
  require(XmAccessibilityAttachShell(bridge, second), "attach second shell");
  XmAccessibilityRefresh(bridge);
  active(bridge, shell, second, dialog, text, shell, app);
  active(bridge, shell, second, dialog, field, second, app);
  active(bridge, shell, second, dialog, dialog, dialog, app);
  active(bridge, shell, second, dialog, menu, shell, app);
  narrow = lines(bridge, text);
  XtVaSetValues(shell, XtNwidth, 500, NULL);
  drain(app, XtDisplay(shell));
  wide = lines(bridge, text);
  require(narrow > wide, "actual resize did not change wrapped line layout");
  printf("Actual multi-shell/transient/override input-focus ACTIVE passed; wrapped lines %d -> %d, all offsets/neighbors passed\n", narrow, wide);
  XmAccessibilityDestroy(bridge);
  XtDestroyWidget(menu); XtDestroyWidget(dialog); XtDestroyWidget(second); XtDestroyWidget(shell);
  return 0;
}
