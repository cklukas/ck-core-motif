#include <Xm/Accessible.h>
#include <Xm/RowColumn.h>
#include <Xm/Skeleton.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return EXIT_FAILURE; } ++checks; } while (0)
int main(int argc, char **argv) {
    XtAppContext app; Widget shell, row, skeleton; XmString label; XmAccessibility *accessibility; XmAccessibleInfo info; XmAccessibleId id;
    (void)argv;
    shell = XtVaAppInitialize(&app, "SkeletonTest", NULL, 0, &argc, argv, NULL, XmNwidth, 320, XmNheight, 100, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0); XtManageChild(row); label = XmStringCreateLocalized((char *)"Loading records");
    skeleton = XmVaCreateManagedSkeleton(row, (char *)"skeleton", XmNskeletonLabel, label, XmNskeletonInterval, 10UL, NULL); XmStringFree(label);
    XtRealizeWidget(shell); XSync(XtDisplay(shell), False); CHECK(XmSkeletonIsAnimating(skeleton)); XmSkeletonStop(skeleton); CHECK(!XmSkeletonIsAnimating(skeleton)); XmSkeletonStart(skeleton); CHECK(XmSkeletonIsAnimating(skeleton));
    XtVaSetValues(skeleton, XmNskeletonReducedMotion, True, NULL); CHECK(!XmSkeletonIsAnimating(skeleton));
    accessibility = XmAccessibilityCreate(shell, "SkeletonTest"); CHECK(accessibility != NULL); id = XmAccessibilityWidget(accessibility, skeleton); CHECK(XmAccessibilityQuery(accessibility, id, &info)); CHECK(info.role == XmACCESSIBLE_PANEL && info.name != NULL && strcmp(info.name, "Loading records") == 0); XmAccessibleInfoClear(&info); XmAccessibilityDestroy(accessibility);
    XtDestroyWidget(shell); XtCloseDisplay(XtDisplay(shell)); XtDestroyApplicationContext(app); printf("PASS skeleton: %u checks\n", checks); return EXIT_SUCCESS;
}
