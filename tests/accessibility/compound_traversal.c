/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* One compound input row owns one tab group. No custom key translations. */
#include <Xm/Display.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int assertions, input_activations, first_activations, second_activations;
static void require(Boolean value, const char *message) {
  ++assertions;
  if (!value) { fprintf(stderr, "%s\n", message); exit(1); }
}
static void activated(Widget widget, XtPointer closure, XtPointer call) {
  (void)widget; (void)call; ++*(unsigned int *)closure;
}
static void drain(XtAppContext app, Display *display) {
  XSync(display, False);
  while (XtAppPending(app)) XtAppProcessEvent(app, XtIMAll);
}
static void step(XtAppContext app, Widget shell, Widget from, Widget to, Boolean reverse) {
  Boolean text = XmIsTextField(from);
  const char *name = text ? (reverse ? "prev-tab-group" : "next-tab-group")
                          : (reverse ? "PrimitivePrevTabGroup" : "PrimitiveNextTabGroup");
  require(XmGetFocusWidget(shell) == from, "incorrect starting focus");
  XtCallActionProc(from, (char *)name, NULL, NULL, 0);
  drain(app, XtDisplay(shell));
  require(XmGetFocusWidget(shell) == to, "compound row traversal skipped a control");
}
int main(int argc, char **argv) {
  XtAppContext app;
  Widget shell, column, row, input, first, second, following;
  Boolean enabled = False;
  require(setlocale(LC_ALL, "") != NULL, "locale unavailable");
  XtSetLanguageProc(NULL, NULL, NULL);
  shell = XtVaAppInitialize(&app, "CompoundTraversal", NULL, 0, &argc, argv, NULL,
                            XmNtitle, "Compound input row traversal", NULL);
  XtVaSetValues(XmGetXmDisplay(XtDisplay(shell)), XmNenableButtonTab, True, NULL);
  XtVaGetValues(XmGetXmDisplay(XtDisplay(shell)), XmNenableButtonTab, &enabled, NULL);
  require(enabled, "button traversal policy was not installed");
  column = XtVaCreateManagedWidget("controls", xmRowColumnWidgetClass, shell,
                                    XmNorientation, XmVERTICAL, NULL);
  row = XtVaCreateManagedWidget("inputRow", xmRowColumnWidgetClass, column,
                                 XmNorientation, XmHORIZONTAL,
                                 XmNnavigationType, XmTAB_GROUP, NULL);
  /* TextField's default XmTAB_GROUP would create a separate nested group;
   * XmNONE joins the field with its adjacent action buttons in this row. */
  input = XtVaCreateManagedWidget("Expression", xmTextFieldWidgetClass, row,
                                   XmNcolumns, 20, XmNnavigationType, XmNONE, NULL);
  first = XtVaCreateManagedWidget("Evaluate", xmPushButtonWidgetClass, row, NULL);
  second = XtVaCreateManagedWidget("Add graph", xmPushButtonWidgetClass, row, NULL);
  following = XtVaCreateManagedWidget("Following field", xmTextFieldWidgetClass,
                                       column, XmNcolumns, 20, NULL);
  XtAddCallback(input, XmNactivateCallback, activated, &input_activations);
  XtAddCallback(first, XmNactivateCallback, activated, &first_activations);
  XtAddCallback(second, XmNactivateCallback, activated, &second_activations);
  XtRealizeWidget(shell);
  drain(app, XtDisplay(shell));
  require(XmProcessTraversal(input, XmTRAVERSE_CURRENT), "initial focus failed");
  drain(app, XtDisplay(shell));
  step(app, shell, input, first, False);
  XtCallActionProc(first, "ArmAndActivate", NULL, NULL, 0);
  require(first_activations == 1 && !input_activations && !second_activations,
          "focused action callback ownership");
  step(app, shell, first, second, False);
  XtCallActionProc(second, "ArmAndActivate", NULL, NULL, 0);
  require(first_activations == 1 && second_activations == 1 && !input_activations,
          "second focused action callback ownership");
  step(app, shell, second, following, False);
  step(app, shell, following, second, True);
  step(app, shell, second, first, True);
  step(app, shell, first, input, True);
  XtSetSensitive(first, False);
  step(app, shell, input, second, False);
  step(app, shell, second, input, True);
  XtDestroyWidget(shell);
  printf("{\"assertions\":%u,\"input_activations\":%u,\"first_activations\":%u,\"second_activations\":%u}\n",
         assertions, input_activations, first_activations, second_activations);
  return 0;
}
