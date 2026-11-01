/* Verify SVG loading through the existing installed pixmap cache. */
#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Xm/Xm.h>

#include "ImageCachI.h"

/* A vector rendered at 2x must contain detail within source-pixel blocks.
 * Nearest-neighbor enlargement repeats each source pixel in a uniform 2x2. */
static int vector_detail(Display *display, Pixmap pixmap, unsigned width,
                         unsigned height)
{
    XImage *image = XGetImage(display, pixmap, 0, 0, width, height,
                              AllPlanes, ZPixmap);
    unsigned x, y, detailed = 0;
    if (!image) return 0;
    for (y = 0; y + 1 < height; y += 2)
        for (x = 0; x + 1 < width; x += 2) {
            unsigned long p = XGetPixel(image, x, y);
            if (p != XGetPixel(image, x + 1, y) ||
                p != XGetPixel(image, x, y + 1) ||
                p != XGetPixel(image, x + 1, y + 1)) ++detailed;
        }
    XDestroyImage(image);
    return detailed >= 8;
}

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

    /* Explicit installed images win over an SVG filename, and still receive
     * bitmap scaling rather than the vector path's already-rendered flag. */
    {
        XImage *installed = XCreateImage(display, DefaultVisualOfScreen(screen),
            1, XYBitmap, 0, NULL, 3, 5, 8, 0);
        Pixmap override;
        if (!installed) return fail("cannot create installed bitmap");
        installed->data = calloc((size_t)installed->bytes_per_line, 5);
        if (!installed->data || !XmInstallImage(installed, argv[1]))
            return fail("cannot install image override");
        override = XmGetScaledPixmap(shell, argv[1], foreground, background,
                                      DefaultDepthOfScreen(screen), 2.0);
        if (override == XmUNSPECIFIED_PIXMAP ||
            !geometry(display, override, &width, &height) ||
            width != 6 || height != 10)
            return fail("installed bitmap precedence/scaling changed");
        XmDestroyPixmap(screen, override);
        if (!XmUninstallImage(installed)) return fail("cannot uninstall override");
        XDestroyImage(installed);
    }

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

    if (!vector_detail(display, scaled, scaled_width, scaled_height))
        return fail("scaled SVG is an enlarged low-resolution bitmap");
    {
        const double scales[] = {0.01, 0.5, 1.5, 3.0};
        const unsigned widths[] = {1, 8, 24, 48};
        const unsigned heights[] = {1, 6, 18, 36};
        unsigned i;
        for (i = 0; i < XtNumber(scales); ++i) {
            Pixmap p = XmGetScaledPixmap(shell, argv[1], foreground, background,
                                         DefaultDepthOfScreen(screen), scales[i]);
            if (p == XmUNSPECIFIED_PIXMAP ||
                !geometry(display, p, &scaled_width, &scaled_height) ||
                scaled_width != widths[i] || scaled_height != heights[i])
                return fail("SVG fractional/small/large scale geometry differs");
            XmDestroyPixmap(screen, p);
        }
        if (XmGetScaledPixmap(shell, argv[1], foreground, background,
                DefaultDepthOfScreen(screen), 1000.0) != XmUNSPECIFIED_PIXMAP)
            return fail("oversized SVG raster accepted");
    }
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
