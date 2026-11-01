/* Exercise the real gallery factory and rendered button icon backgrounds. */
#include "native-window.h"
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Toolbar.h>
#include <Xm/LabelP.h>
#include <Xm/PushBP.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>

extern Widget gallery_create_component_example(Widget, WidgetClass, const char *,
                                                XtCallbackProc, XtPointer);
extern void gallery_refresh_icon_resources(void);
static int profile;
static unsigned action;
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL toolbar icon: profile=%d action=%u line=%d: %s\n", profile, action, __LINE__, #c); exit(1); } } while (0)
static void pump(XtAppContext app, Display *display)
{
    for (unsigned i = 0; i < 4; ++i) {
        XSync(display, False);
        while (XtAppPending(app)) XtAppProcessEvent(app, XtIMAll);
    }
}
static Pixel corner(Display *display, Drawable drawable, int x, int y)
{
    XImage *image = XGetImage(display, drawable, x, y, 1, 1, AllPlanes, ZPixmap);
    CHECK(image);
    Pixel result = XGetPixel(image, 0, 0);
    XDestroyImage(image);
    return result;
}
static void check_button(Widget button, Boolean visible, XtAppContext app)
{
    Display *display = XtDisplay(button);
    Pixel background, armed;
    Pixmap normal, pressed;
    XtVaGetValues(button, XmNbackground, &background, XmNarmColor, &armed,
        XmNlabelPixmap, &normal, XmNarmPixmap, &pressed, NULL);
    CHECK(normal != None && normal != XmUNSPECIFIED_PIXMAP);
    CHECK(pressed != None && pressed != XmUNSPECIFIED_PIXMAP);
    CHECK(corner(display, normal, 0, 0) == background);
    CHECK(corner(display, pressed, 0, 0) == armed);
    if (!visible) return;
    NativeWaitForViewable(button);
    XmLabelWidget label = (XmLabelWidget)button;
    int x = label->label.TextRect.x + label->label.PixmapRect.x;
    int y = label->label.TextRect.y + label->label.PixmapRect.y;
    CHECK(corner(display, XtWindow(button), x, y) == background);
    XEvent event = {0};
    event.xbutton.type = ButtonPress;
    event.xbutton.display = display;
    event.xbutton.window = XtWindow(button);
    event.xbutton.button = Button1;
    event.xbutton.x = x;
    event.xbutton.y = y;
    XtCallActionProc(button, "Arm", &event, NULL, 0);
    pump(app, display);
    CHECK(((XmPushButtonWidget)button)->pushbutton.armed);
    CHECK(corner(display, XtWindow(button), x, y) == armed);
    XtCallActionProc(button, "Disarm", &event, NULL, 0);
    pump(app, display);
    CHECK(corner(display, XtWindow(button), x, y) == background);
    XtSetSensitive(button, False);
    pump(app, display);
    CHECK(corner(display, XtWindow(button), x, y) == background);
    XtSetSensitive(button, True);
    pump(app, display);
}
int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell = XtVaAppInitialize(&app, "ToolbarIconSurfaces", NULL, 0,
                                     &argc, argv, NULL, NULL);
    Widget host = XtVaCreateManagedWidget("host", xmBulletinBoardWidgetClass,
        shell, XmNwidth, 1500, XmNheight, 400, NULL);
    Widget toolbar = gallery_create_component_example(host, xmToolbarWidgetClass,
                                                       "toolbar", NULL, NULL);
    CHECK(toolbar && XmToolbarGetActionCount(toolbar) == 4);
    CHECK(XmToolbarGetActionButton(NULL, 0, False) == NULL);
    CHECK(XmToolbarGetActionButton(host, 0, False) == NULL);
    CHECK(XmToolbarGetActionButton(toolbar, 4, False) == NULL);
    XtRealizeWidget(shell);
    NativeWaitForViewable(shell);
    const XmAppearanceProfile profiles[] = {XmAPPEARANCE_MODERN_LIGHT,
        XmAPPEARANCE_MODERN_DARK, XmAPPEARANCE_HIGH_CONTRAST, XmAPPEARANCE_CLASSIC};
    for (unsigned scale = 0; scale < 2; ++scale) {
        CHECK(XmSetDisplayScaleOverride(XtDisplay(shell), scale ? 1.5 : 1.0));
        for (unsigned p = 0; p < XtNumber(profiles); ++p) {
            profile = profiles[p];
            CHECK(XmApplyAppearanceProfile(XtDisplay(shell), profiles[p]));
            gallery_refresh_icon_resources();
            XtVaSetValues(toolbar, XmNwidth, 1200, NULL);
            pump(app, XtDisplay(shell));
            CHECK(XmToolbarGetVisibleActionCount(toolbar) == 4);
            for (action = 0; action < 4; ++action) {
                check_button(XmToolbarGetActionButton(toolbar, action, False), True, app);
                check_button(XmToolbarGetActionButton(toolbar, action, True), False, app);
            }
        }
    }
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    puts("PASS toolbar icon surfaces: normal, pressed, disabled and overflow; four profiles and two scales");
    return 0;
}
