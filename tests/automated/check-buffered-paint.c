/* Exercise real X drawables: intermediate frames must stay off the front. */
#include <Xm/XmAll.h>
#include <Xm/XmP.h>
#include "PaintI.h"
#include <X11/Xutil.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(TEST_NO_DBE_EXTENSION) || defined(TEST_DBE_ALLOC_FAILURE)
#include <X11/extensions/Xdbe.h>
#endif
#ifdef TEST_NO_DBE_EXTENSION
Status XdbeQueryExtension(Display *display, int *major, int *minor)
{
    (void)display; (void)major; (void)minor;
    return 0;
}
#endif
#ifdef TEST_DBE_ALLOC_FAILURE
XdbeBackBuffer XdbeAllocateBackBufferName(Display *display, Window window,
                                         XdbeSwapAction action)
{
    (void)action;
    /* Produce a real asynchronous allocation-request error at this serial. */
    return XCreatePixmap(display, window, 1, 1, 0);
}
#endif

static int unexpected_error(Display *display, XErrorEvent *event)
{
    (void)display;
    fprintf(stderr, "Unexpected X error: %u request %u\n", event->error_code, event->request_code);
    abort();
    return 0;
}
static Pixel before, after;
static int frames;
static Boolean buffered = True;
static int expected_backend = 2;
static Pixel pixel(Display *display, Drawable drawable, int x, int y)
{
    XImage *image = XGetImage(display, drawable, x, y, 1, 1, AllPlanes, ZPixmap);
    assert(image);
    Pixel result = XGetPixel(image, 0, 0);
    XDestroyImage(image);
    return result;
}
static void paint(Widget widget, XEvent *event, Region region)
{
    Display *display = XtDisplay(widget);
    Drawable target = _XmPaintDrawable(widget);
    GC gc = XCreateGC(display, target, 0, NULL);
    (void)event; (void)region;
    assert((target != XtWindow(widget)) == buffered);
    assert(_XmPaintBufferKind(widget) == expected_backend);
    XSetForeground(display, gc, after);
    XFillRectangle(display, target, gc, 0, 0, widget->core.width, widget->core.height);
    XSync(display, False);
    if (buffered) assert(pixel(display, XtWindow(widget), 4, 4) == before);
    assert(pixel(display, target, 4, 4) == after);
    XFreeGC(display, gc);
    ++frames;
}
static void frame(Widget widget)
{
    Display *display = XtDisplay(widget);
    XWindowAttributes attributes;
    XGetWindowAttributes(display, XtWindow(widget), &attributes);
    for (int i = 0; attributes.map_state != IsViewable && i < 200; ++i) {
        while (XtAppPending(XtWidgetToApplicationContext(widget)))
            XtAppProcessEvent(XtWidgetToApplicationContext(widget), XtIMAll);
        struct timespec delay = {0, 10000000};
        nanosleep(&delay, NULL);
        XGetWindowAttributes(display, XtWindow(widget), &attributes);
    }
    assert(attributes.map_state == IsViewable);
    GC gc = XCreateGC(display, XtWindow(widget), 0, NULL);
    XSetForeground(display, gc, before);
    XFillRectangle(display, XtWindow(widget), gc, 0, 0, widget->core.width, widget->core.height);
    XFreeGC(display, gc);
    _XmBufferedPaint(widget, paint, NULL, NULL);
    assert(XSetErrorHandler(unexpected_error) == unexpected_error);
    XSync(display, False);
    assert(pixel(display, XtWindow(widget), 4, 4) == after);
}
int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell = XtVaAppInitialize(&app, "PaintRegression", NULL, 0,
                                     &argc, argv, NULL, NULL);
    buffered = !(argc > 1 && !strcmp(argv[1], "direct"));
    expected_backend = !buffered ? 0 : (argc > 1 && !strcmp(argv[1], "pixmap") ? 1 : 2);
    Widget widget = XtVaCreateManagedWidget("surface", xmDrawingAreaWidgetClass,
        shell, XmNwidth, 96, XmNheight, 64, NULL);
    XSetErrorHandler(unexpected_error);
    _XmBufferedPaint(NULL, paint, NULL, NULL);
    XtRealizeWidget(shell);
    XMapRaised(XtDisplay(shell), XtWindow(shell));
    XSync(XtDisplay(shell), False);
    before = BlackPixelOfScreen(XtScreen(widget));
    after = WhitePixelOfScreen(XtScreen(widget));
    for (int i = 0; i < 10; ++i) frame(widget);
    XtVaSetValues(widget, XmNwidth, 120, XmNheight, 80, NULL);
    XSync(XtDisplay(shell), False);
    frame(widget);
    XtUnrealizeWidget(shell);
    XtManageChild(widget);
    XtRealizeWidget(shell);
    XMapRaised(XtDisplay(shell), XtWindow(shell));
    XSync(XtDisplay(shell), False);
    frame(widget);
    if (buffered) {
    /* XtSetValues must not clear a buffered control before its frame runs. */
    Widget progress = XtVaCreateManagedWidget("progress", xmProgressBarWidgetClass,
        widget, XmNwidth, 80, XmNheight, 24, XmNprogressValue, 20, NULL);
    XSync(XtDisplay(widget), False);
    XtClass(progress)->core_class.expose(progress, NULL, NULL);
    XImage *old_image = XGetImage(XtDisplay(widget), XtWindow(progress), 0, 0,
                                 80, 24, AllPlanes, ZPixmap);
    assert(old_image);
    XtVaSetValues(progress, XmNprogressValue, 80, NULL);
    XSync(XtDisplay(widget), False);
    XImage *pending_image = XGetImage(XtDisplay(widget), XtWindow(progress), 0, 0,
                                     80, 24, AllPlanes, ZPixmap);
    assert(pending_image);
    for (int y = 0; y < 24; ++y) for (int x = 0; x < 80; ++x)
        assert(XGetPixel(old_image, x, y) == XGetPixel(pending_image, x, y));
    XDestroyImage(old_image); XDestroyImage(pending_image);
    }
    /* A queued repaint must not outlive its widget. */
    _XmPaintInvalidate(widget);
    _XmPaintInvalidate(widget);
    XSync(XtDisplay(widget), False);
    assert(pixel(XtDisplay(widget), XtWindow(widget), 90, 40) == after);
    XtDestroyWidget(shell);
    while (XtAppPending(app)) XtAppProcessEvent(app, XtIMAll);
    XtDestroyApplicationContext(app);
    assert(frames == 12);
    puts("PASS buffered painting: front isolation, presentation, resize, unrealize and teardown");
    return 0;
}
