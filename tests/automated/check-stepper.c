#include <Xm/Stepper.h>
#include <Xm/Accessible.h>
#include <Xm/BulletinB.h>
#include <stdio.h>
#include <stdlib.h>
static unsigned checks, changes;
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); return EXIT_FAILURE; } ++checks; } while (0)
static void changed(Widget w, XtPointer c, XtPointer data) { (void)w; (void)c; (void)data; ++changes; }
int main(int argc, char **argv) {
    XtAppContext app; Widget shell, board, stepper; XmAccessibility *a; XmAccessibleInfo info; XmAccessibleId id; Display *display; Dimension width, height; int minimum, maximum;
    shell = XtVaAppInitialize(&app, "StepperTest", NULL, 0, &argc, argv, NULL, XmNwidth, 240, XmNheight, 80, NULL);
    board = XmCreateBulletinBoard(shell, (char *)"board", NULL, 0); XtManageChild(board);
    stepper = XmVaCreateManagedStepper(board, (char *)"stepper", XmNstepperMinimum, 0, XmNstepperMaximum, 4, XmNstepperValue, 2, XmNstepperStep, 2, XmNstepperWrap, True, XmNwidth, 100, XmNheight, 36, NULL);
    XtAddCallback(stepper, XmNstepperValueChangedCallback, changed, NULL); XtRealizeWidget(shell); display = XtDisplay(shell); XSync(display, False);
    XtVaGetValues(stepper, XmNwidth, &width, XmNheight, &height, NULL); CHECK(width == 100 && height == 36);
    XmStepperGetRange(stepper, &minimum, &maximum); CHECK(minimum == 0 && maximum == 4); CHECK(XmStepperGetValue(stepper) == 2);
    XmStepperStep(stepper, 1, True); CHECK(XmStepperGetValue(stepper) == 4 && changes == 1);
    XmStepperStep(stepper, 1, True); CHECK(XmStepperGetValue(stepper) == 0 && changes == 2);
    XmStepperSetValue(stepper, 99, True); CHECK(XmStepperGetValue(stepper) == 4 && changes == 3);
    a = XmAccessibilityCreate(shell, "StepperTest"); CHECK(a != NULL); id = XmAccessibilityWidget(a, stepper); CHECK(id != XmACCESSIBLE_NONE); CHECK(XmAccessibilityQuery(a, id, &info)); CHECK(info.has_value && info.minimum_value == 0 && info.maximum_value == 4 && info.current_value == 4); XmAccessibleInfoClear(&info); XmAccessibilityDestroy(a);
    XtDestroyWidget(stepper); printf("PASS stepper: %u checks\n", checks); (void)display; return EXIT_SUCCESS;
}
