/* Verify SVG loading through the existing installed pixmap cache. */
#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>

#include "ImageCachI.h"

static int
geometry(Display *display, Pixmap pixmap, unsigned int *width,
         unsigned int *height)
{
    Window root;
    int x;
    int y;
    unsigned int border;
    unsigned int depth;

    return XGetGeometry(display, pixmap, &root, &x, &y, width, height,
                        &border, &depth);
}

static int
fail(const char *message)
{
    fprintf(stderr, "%s\n", message);
    return EXIT_FAILURE;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Display *display;
    Screen *screen;
    Pixel foreground;
    Pixel background;
    Pixmap first;
    Pixmap second;
    Pixmap scaled;
    Pixmap invalid;
    unsigned int width;
    unsigned int height;
    unsigned int scaled_width;
    unsigned int scaled_height;

    if (argc != 3)
        return fail("usage: check-svg-image VALID.svg INVALID.svg");
    shell = XtVaAppInitialize(&app, "SvgContract", NULL, 0, &argc, argv,
                              NULL, NULL);
    display = XtDisplay(shell);
    screen = XtScreen(shell);
    foreground = BlackPixelOfScreen(screen);
    background = WhitePixelOfScreen(screen);

    first = XmGetPixmapByDepth(screen, argv[1], foreground, background,
                               DefaultDepthOfScreen(screen));
    if (getenv("XM_EXPECT_SVG_UNAVAILABLE") != NULL) {
        if (first != XmUNSPECIFIED_PIXMAP)
            return fail("disabled SVG build unexpectedly loaded an SVG");
        XtDestroyWidget(shell);
        XtCloseDisplay(display);
        puts("PASS SVG unavailable is reported by the disabled build");
        return EXIT_SUCCESS;
    }
    if (first == XmUNSPECIFIED_PIXMAP ||
        !geometry(display, first, &width, &height) || width != 16 || height != 12)
        return fail("valid SVG did not produce its intrinsic dimensions");
    second = XmGetPixmapByDepth(screen, argv[1], foreground, background,
                                DefaultDepthOfScreen(screen));
    if (second != first)
        return fail("identical SVG lookups did not reuse the pixmap cache");
    if (!XmDestroyPixmap(screen, first))
        return fail("first cached SVG reference was not releasable");
    if (!geometry(display, second, &width, &height))
        return fail("cached SVG was retired before its final reference");

    scaled = XmGetScaledPixmap(shell, argv[1], foreground, background,
                                DefaultDepthOfScreen(screen), 2.0);
    if (scaled == XmUNSPECIFIED_PIXMAP || scaled == second ||
        !geometry(display, scaled, &scaled_width, &scaled_height) ||
        scaled_width != width * 2 || scaled_height != height * 2)
        return fail("scaled SVG did not use a distinct doubled cache entry");

    invalid = XmGetPixmapByDepth(screen, argv[2], foreground, background,
                                 DefaultDepthOfScreen(screen));
    if (invalid != XmUNSPECIFIED_PIXMAP)
        return fail("malformed SVG was accepted");
    if (!XmDestroyPixmap(screen, second) || !XmDestroyPixmap(screen, scaled))
        return fail("SVG cache entries were not releasable");

    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    puts("PASS SVG image loading, scaling, cache reuse and invalid input");
    return EXIT_SUCCESS;
}
