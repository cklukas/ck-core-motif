#include <Xm/Accessible.h>
#include <Xm/BulletinB.h>
#include <Xm/ProgressRing.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned checks;
static unsigned changes;
static int callback_value;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return EXIT_FAILURE; \
    } \
    ++checks; \
} while (0)

static void changed(Widget widget, XtPointer closure, XtPointer data) {
    XmProgressBarCallbackStruct *callback =
        (XmProgressBarCallbackStruct *)data;
    (void)widget; (void)closure;
    ++changes;
    callback_value = callback->value;
}

int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell, board, ring;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId id;
    XmString label;
    Display *display;
    int minimum, maximum;
    Dimension width, height;

    shell = XtVaAppInitialize(&app, "ProgressRingTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 280, XmNheight, 100, NULL);
    board = XmCreateBulletinBoard(shell, (char *)"board", NULL, 0);
    XtManageChild(board);
    label = XmStringCreateLocalized((char *)"Download");
    ring = XmVaCreateManagedProgressRing(
        board, (char *)"ring", XmNprogressLabelString, label,
        XmNprogressMinimum, 10, XmNprogressMaximum, 110,
        XmNprogressValue, 60, XmNwidth, 220, XmNheight, 72, NULL);
    XmStringFree(label);
    XtAddCallback(ring, XmNprogressValueChangedCallback, changed, NULL);

    XtRealizeWidget(shell);
    display = XtDisplay(shell);
    XSync(display, False);
    XtVaGetValues(ring, XmNwidth, &width, XmNheight, &height, NULL);
    CHECK(width == 220 && height == 72);
    XmProgressRingGetRange(ring, &minimum, &maximum);
    CHECK(minimum == 10 && maximum == 110);
    CHECK(XmProgressRingGetValue(ring) == 60);
    XmProgressRingSetValue(ring, 75, True);
    CHECK(changes == 1 && callback_value == 75);
    XmProgressRingSetValue(ring, 1000, True);
    CHECK(XmProgressRingGetValue(ring) == 110 && changes == 2);
    XmProgressRingSetRange(ring, 20, 40, True);
    CHECK(XmProgressRingGetValue(ring) == 40 && changes == 3);
    XmProgressRingSetIndeterminate(ring, True, True);
    CHECK(XmProgressRingGetIndeterminate(ring) && changes == 4);
    CHECK(XmProcessTraversal(ring, XmTRAVERSE_CURRENT));
    CHECK(XmGetFocusWidget(shell) == ring);

    accessibility = XmAccessibilityCreate(shell, "ProgressRingTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, ring);
    CHECK(id != XmACCESSIBLE_NONE);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_PROGRESS_BAR && info.has_value);
    CHECK(info.minimum_value == 20 && info.maximum_value == 40 &&
          info.current_value == 40);
    CHECK(info.states & XmACCESSIBLE_STATE(32));
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS progress ring: %u checks\n", checks);
    return EXIT_SUCCESS;
}
