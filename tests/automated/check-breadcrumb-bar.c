#include <Xm/Accessible.h>
#include <Xm/BreadcrumbBar.h>
#include <Xm/BulletinB.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned checks, changes, activations;
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); return EXIT_FAILURE; } ++checks; } while (0)
static void changed(Widget w, XtPointer c, XtPointer data) { XmBreadcrumbBarCallbackStruct *cb = (XmBreadcrumbBarCallbackStruct *)data; (void)w; (void)c; if (cb) ++changes; }
static void activated(Widget w, XtPointer c, XtPointer data) { XmBreadcrumbBarCallbackStruct *cb = (XmBreadcrumbBarCallbackStruct *)data; (void)w; (void)c; if (cb && cb->id) ++activations; }

int main(int argc, char **argv) {
    XtAppContext app; Widget shell, board, bar; XmAccessibility *a; XmAccessibleInfo info; XmAccessibleId id; Dimension width; int one = 1, two = 2; XmBreadcrumbSegment segments[3];
    shell = XtVaAppInitialize(&app, "BreadcrumbTest", NULL, 0, &argc, argv, NULL, XmNwidth, 360, XmNheight, 90, NULL);
    board = XmCreateBulletinBoard(shell, (char *)"board", NULL, 0); XtManageChild(board);
    bar = XmCreateBreadcrumbBar(board, (char *)"bar", NULL, 0);
    XtVaSetValues(bar, XmNbreadcrumbMaxVisible, 2, XmNbreadcrumbRtl, False, XmNwidth, 260, XmNheight, 30, NULL);
    XtAddCallback(bar, XmNbreadcrumbCurrentChangedCallback, changed, NULL); XtAddCallback(bar, XmNbreadcrumbActivateCallback, activated, NULL);
    segments[0].label = "Home"; segments[0].id = &one; segments[1].label = "Reports"; segments[1].id = &two; segments[2].label = "Quarterly"; segments[2].id = NULL;
    XmBreadcrumbBarSetSegments(bar, segments, 3); XtManageChild(bar); XtRealizeWidget(shell); XSync(XtDisplay(shell), False);
    XtVaGetValues(bar, XmNwidth, &width, NULL); CHECK(width == 260); CHECK(XmBreadcrumbBarGetSegmentCount(bar) == 3); CHECK(XmBreadcrumbBarGetCurrentIndex(bar) == 2);
    XmBreadcrumbBarSetCurrentIndex(bar, 1, True); CHECK(XmBreadcrumbBarGetCurrentIndex(bar) == 1 && changes == 1);
    XmBreadcrumbBarSetCurrentIndex(bar, 0, True); CHECK(changes == 2);
    a = XmAccessibilityCreate(shell, "BreadcrumbTest"); CHECK(a != NULL); id = XmAccessibilityWidget(a, bar); CHECK(id != XmACCESSIBLE_NONE); CHECK(XmAccessibilityQuery(a, id, &info)); CHECK(info.role == XmACCESSIBLE_LIST && info.has_selection && info.has_value && info.current_value == 0); XmAccessibleInfoClear(&info); XmAccessibilityDestroy(a);
    XtDestroyWidget(bar); CHECK(activations == 0); printf("PASS breadcrumb bar: %u checks\n", checks); return EXIT_SUCCESS;
}
