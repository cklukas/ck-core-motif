#include <Xm/Accessible.h>
#include <Xm/BulletinB.h>
#include <Xm/RatingControl.h>
#include <stdio.h>
#include <stdlib.h>
static unsigned checks, changes;
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); return EXIT_FAILURE; } ++checks; } while (0)
static void changed(Widget w, XtPointer c, XtPointer data) { (void)w; (void)c; (void)data; ++changes; }
int main(int argc, char **argv) {
    XtAppContext app; Widget shell, board, rating; XmAccessibility *a; XmAccessibleInfo info; XmAccessibleId id; Dimension width, height;
    shell = XtVaAppInitialize(&app, "RatingTest", NULL, 0, &argc, argv, NULL, XmNwidth, 300, XmNheight, 100, NULL);
    board = XmCreateBulletinBoard(shell, (char *)"board", NULL, 0); XtManageChild(board);
    rating = XmCreateRatingControl(board, (char *)"rating", NULL, 0);
    XtVaSetValues(rating, XmNratingCount, 5, XmNratingValue, 2, XmNwidth, 220, XmNheight, 40, NULL);
    XtAddCallback(rating, XmNratingValueChangedCallback, changed, NULL); XtManageChild(rating); XtRealizeWidget(shell); XSync(XtDisplay(shell), False);
    XtVaGetValues(rating, XmNwidth, &width, XmNheight, &height, NULL); CHECK(width == 220 && height == 40); CHECK(XmRatingControlGetValue(rating) == 2);
    XmRatingControlSetValue(rating, 5, True); CHECK(XmRatingControlGetValue(rating) == 5 && changes == 1);
    XmRatingControlSetValue(rating, 99, True); CHECK(XmRatingControlGetValue(rating) == 5 && changes == 2);
    a = XmAccessibilityCreate(shell, "RatingTest"); CHECK(a != NULL); id = XmAccessibilityWidget(a, rating); CHECK(id != XmACCESSIBLE_NONE); CHECK(XmAccessibilityQuery(a, id, &info)); CHECK(info.has_value && info.minimum_value == 0 && info.maximum_value == 5 && info.current_value == 5); XmAccessibleInfoClear(&info); XmAccessibilityDestroy(a); XtDestroyWidget(rating);
    printf("PASS rating control: %u checks\n", checks); return EXIT_SUCCESS;
}
