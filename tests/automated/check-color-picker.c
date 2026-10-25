#include <Xm/Accessible.h>
#include <Xm/BulletinB.h>
#include <Xm/ColorPicker.h>
#include <stdio.h>
#include <stdlib.h>
static unsigned checks, previews, changes;
static unsigned x_errors;
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); return EXIT_FAILURE; } ++checks; } while (0)
static void preview(Widget w, XtPointer c, XtPointer data) { (void)w; (void)c; (void)data; ++previews; }
static void changed(Widget w, XtPointer c, XtPointer data) { (void)w; (void)c; (void)data; ++changes; }
static int x_error(Display *display, XErrorEvent *event) {
    (void)display; (void)event; ++x_errors; return 0;
}
int main(int argc, char **argv) {
    XtAppContext app; Widget shell, board, picker; XmAccessibility *a;
    XmAccessibleInfo info; XmAccessibleId id; Dimension width, height;
    shell = XtVaAppInitialize(&app, "ColorPickerTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 300, XmNheight, 100, NULL);
    board = XmCreateBulletinBoard(shell, (char *)"board", NULL, 0); XtManageChild(board);
    picker = XmCreateColorPicker(board, (char *)"picker", NULL, 0);
    XtVaSetValues(picker, XmNcolorPickerValue, 0x11223344UL, XmNwidth, 220,
                  XmNheight, 40, NULL);
    XtAddCallback(picker, XmNcolorPickerPreviewCallback, preview, NULL);
    XtAddCallback(picker, XmNcolorPickerValueChangedCallback, changed, NULL);
    XtManageChild(picker); XtRealizeWidget(shell);
    XSetErrorHandler(x_error);
    XClearArea(XtDisplay(picker), XtWindow(picker), 0, 0, 0, 0, True);
    XSync(XtDisplay(shell), False);
    while (XtAppPending(app) & XtIMXEvent)
        XtAppProcessEvent(app, XtIMXEvent);
    CHECK(x_errors == 0);
    XtVaGetValues(picker, XmNwidth, &width, XmNheight, &height, NULL);
    CHECK(width == 220 && height == 40); CHECK(XmColorPickerGetValue(picker) == 0x11223344UL);
    XmColorPickerPreviewValue(picker, 0xaabbccddUL);
    CHECK(previews == 1 && XmColorPickerGetValue(picker) == 0x11223344UL);
    XmColorPickerSetValue(picker, 0xaabbccddUL, True);
    CHECK(changes == 1 && XmColorPickerGetValue(picker) == 0xaabbccddUL);
    a = XmAccessibilityCreate(shell, "ColorPickerTest"); CHECK(a != NULL);
    id = XmAccessibilityWidget(a, picker); CHECK(id != XmACCESSIBLE_NONE);
    CHECK(XmAccessibilityQuery(a, id, &info));
    CHECK(info.has_value && info.current_value == (double)0xaabbccddUL && info.value_text != NULL);
    XmAccessibleInfoClear(&info); XmAccessibilityDestroy(a); XtDestroyWidget(picker);
    printf("PASS color picker: %u checks\n", checks); return EXIT_SUCCESS;
}
