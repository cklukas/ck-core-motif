/* Actual X pixels: modern arrows must fit within a centered, bounded glyph. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Xutil.h>
#include <stdio.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

static void flush(XtAppContext app, Display *dpy)
{
    XSync(dpy, False);
    while (XtAppPending(app)) XtAppProcessEvent(app, XtIMAll);
    XSync(dpy, False);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Display *dpy;
    const double scales[] = {1, 1.5, 2, 3};
    const unsigned char dirs[] = {XmARROW_UP, XmARROW_DOWN, XmARROW_LEFT, XmARROW_RIGHT};
    XtToolkitInitialize();
    app = XtCreateApplicationContext();
    dpy = XtOpenDisplay(app, NULL, "bounds", "Bounds", NULL, 0, &argc, argv);
    CHECK(dpy);
    for (unsigned s = 0; s < XtNumber(scales); ++s)
    for (unsigned dark = 0; dark < 2; ++dark)
    for (unsigned compact = 0; compact < 2; ++compact) {
        Widget shell, board, form, explicit_form;
        Dimension shadow = 99;
        CHECK(XmSetDisplayScaleOverride(dpy, scales[s]));
        CHECK(XmSetAppearanceProfile(dpy, dark ? XmAPPEARANCE_MODERN_DARK : XmAPPEARANCE_MODERN_LIGHT));
        CHECK(XmSetAppearanceDensity(dpy, compact ? XmAPPEARANCE_DENSITY_COMPACT : XmAPPEARANCE_DENSITY_COMFORTABLE));
        shell = XtAppCreateShell("bounds", "Bounds", applicationShellWidgetClass, dpy, NULL, 0);
        board = XtVaCreateManagedWidget("board", xmBulletinBoardWidgetClass, shell,
            XmNwidth, 400, XmNheight, 300, XmNresizePolicy, XmRESIZE_NONE, NULL);
        XtVaGetValues(board, XmNshadowThickness, &shadow, NULL);
        CHECK(shadow == 0);
        form = XtVaCreateWidget("form", xmFormWidgetClass, board, NULL);
        XtVaGetValues(form, XmNshadowThickness, &shadow, NULL);
        CHECK(shadow == 0);
        explicit_form = XtVaCreateWidget("explicit", xmFormWidgetClass, board,
            XmNshadowThickness, 7, NULL);
        XtVaGetValues(explicit_form, XmNshadowThickness, &shadow, NULL);
        CHECK(shadow == 7);
        XtRealizeWidget(shell);
        for (unsigned gadget = 0; gadget < 2; ++gadget)
        for (unsigned dir = 0; dir < XtNumber(dirs); ++dir)
        for (unsigned tiny = 0; tiny < 2; ++tiny) {
            int width = tiny ? 8 : (int)(90 * scales[s]);
            int height = tiny ? 8 : (int)(60 * scales[s]);
            int side = tiny ? 0 : (int)((compact ? 16 : 18) * scales[s]);
            Widget arrow = XtVaCreateManagedWidget("arrow",
                gadget ? xmArrowButtonGadgetClass : xmArrowButtonWidgetClass, board,
                XmNx, 10, XmNy, 10, XmNwidth, width, XmNheight, height,
                XmNhighlightThickness, 0, XmNshadowThickness, 2,
                XmNarrowDirection, dirs[dir], NULL);
            for (unsigned disabled = 0; disabled < 2; ++disabled) {
                Pixel background;
                XImage *image;
                unsigned ink = 0;
                XtSetSensitive(arrow, !disabled);
                XClearArea(dpy, XtWindow((gadget ? board : arrow)), 0, 0, 0, 0, True);
                flush(app, dpy);
                XtVaGetValues(arrow, XmNbackground, &background, NULL);
                image = XGetImage(dpy, XtWindow((gadget ? board : arrow)),
                    gadget ? 10 : 0, gadget ? 10 : 0, width, height, AllPlanes, ZPixmap);
                CHECK(image);
                for (int y = 2; y < height - 2; ++y)
                for (int x = 2; x < width - 2; ++x) {
                    if (XGetPixel(image, x, y) == background) continue;
                    ++ink;
                    if (x < (width-side)/2 || x >= (width-side)/2+side ||
                        y < (height-side)/2 || y >= (height-side)/2+side)
                        fprintf(stderr, "scale=%g dark=%u compact=%u gadget=%u dir=%u tiny=%u disabled=%u pixel=(%d,%d) size=%dx%d side=%d background=%lu pixel=%lu\n", scales[s], dark, compact, gadget, dir, tiny, disabled, x,y,width,height,side,background,XGetPixel(image,x,y));
                    CHECK(x >= (width - side)/2 && x < (width - side)/2 + side);
                    CHECK(y >= (height - side)/2 && y < (height - side)/2 + side);
                }
                CHECK(tiny ? ink == 0 : ink > 0);
                XDestroyImage(image);
            }
            XtDestroyWidget(arrow);
            flush(app, dpy);
        }
        XtDestroyWidget(shell);
        flush(app, dpy);
    }
    XtCloseDisplay(dpy);
    XtDestroyApplicationContext(app);
    puts("PASS XmArrowButton modern glyph bounds");
    return 0;
}
