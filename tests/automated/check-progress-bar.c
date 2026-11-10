#include <Xm/Accessible.h>
#include <Xm/BulletinB.h>
#include <Xm/ProgressBar.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned checks;
static unsigned changes;
static unsigned destroyed;
static int callback_value;
static Boolean callback_reentered;

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

static void destroy_on_change(Widget widget, XtPointer closure, XtPointer data) {
    (void)closure; (void)data;
    ++destroyed;
    /* Exercise callback re-entry without retaining call-data after return. */
    callback_reentered = XmProgressBarGetValue(widget) == 25;
}

static void drain(XtAppContext app, Display *display) {
    XSync(display, False);
    while (XtAppPending(app)) XtAppProcessEvent(app, XtIMAll);
    XSync(display, False);
}

static Pixel pixel_at(Display *display, Widget widget, int x, int y) {
    XImage *image = XGetImage(display, XtWindow(widget), x, y, 1, 1,
                              AllPlanes, ZPixmap);
    Pixel pixel;
    if (!image) return 0;
    pixel = XGetPixel(image, 0, 0);
    XDestroyImage(image);
    return pixel;
}

int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell, board, progress, doomed, rtl;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId id;
    XmString label;
    Display *display;
    XColor exact, screen;
    int minimum, maximum;
    Dimension width, height;

    shell = XtVaAppInitialize(&app, "ProgressBarTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 360, XmNheight, 120, NULL);
    board = XmCreateBulletinBoard(shell, (char *)"board", NULL, 0);
    XtManageChild(board);
    label = XmStringCreateLocalized((char *)"Download");
    progress = XmVaCreateManagedProgressBar(
        board, (char *)"progress", XmNprogressLabelString, label,
        XmNprogressMinimum, 10, XmNprogressMaximum, 110,
        XmNprogressValue, 60, XmNwidth, 240, XmNheight, 64, NULL);
    XmStringFree(label);
    XtAddCallback(progress, XmNprogressValueChangedCallback, changed, NULL);
    doomed = XmCreateProgressBar(board, (char *)"doomed", NULL, 0);
    XtManageChild(doomed);
    XtAddCallback(doomed, XmNprogressValueChangedCallback, destroy_on_change,
                  NULL);

    XtRealizeWidget(shell);
    display = XtDisplay(shell); drain(app, display);
    XtVaGetValues(progress, XmNwidth, &width, XmNheight, &height, NULL);
    CHECK(width == 240 && height == 64);
    XmProgressBarGetRange(progress, &minimum, &maximum);
    CHECK(minimum == 10 && maximum == 110);
    CHECK(XmProgressBarGetValue(progress) == 60);
    XmProgressBarSetValue(progress, 75, True);
    CHECK(changes == 1 && callback_value == 75);
    XmProgressBarSetValue(progress, 1000, True);
    CHECK(XmProgressBarGetValue(progress) == 110 && changes == 2);
    XmProgressBarSetRange(progress, 20, 40, True);
    CHECK(XmProgressBarGetValue(progress) == 40 && changes == 3);
    XmProgressBarSetIndeterminate(progress, True, True);
    CHECK(XmProgressBarGetIndeterminate(progress) && changes == 4);
    CHECK(XmProcessTraversal(progress, XmTRAVERSE_CURRENT));
    CHECK(XmGetFocusWidget(shell) == progress);
    XtSetSensitive(progress, False);
    CHECK(!XtIsSensitive(progress));

    CHECK(XAllocNamedColor(display, DefaultColormapOfScreen(XtScreen(shell)),
                           "red", &screen, &exact));
    rtl = XmVaCreateManagedProgressBar(
        board, (char *)"rtl", XmNlayoutDirection, XmRIGHT_TO_LEFT,
        XmNhighlightColor, exact.pixel, XmNprogressValue, 50,
        XmNwidth, 240, XmNheight, 40, XmNprogressTextVisible, False, NULL);
    drain(app, display);
    CHECK(pixel_at(display, rtl, 11, 20) != exact.pixel);
    CHECK(pixel_at(display, rtl, 229, 20) == exact.pixel);

    accessibility = XmAccessibilityCreate(shell, "ProgressBarTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, progress);
    CHECK(id != XmACCESSIBLE_NONE);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_PROGRESS_BAR && info.has_value);
    CHECK(info.minimum_value == 20 && info.maximum_value == 40 &&
          info.current_value == 40);
    CHECK(info.action_count == 0);
    XmAccessibleInfoClear(&info);
    XmProgressBarSetValue(doomed, 25, True);
    CHECK(destroyed == 1 && callback_reentered);
    XmAccessibilityDestroy(accessibility);
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS progress bar: %u checks\n", checks);
    return EXIT_SUCCESS;
}
