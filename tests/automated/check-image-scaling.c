#include "ImageCachI.h"
#include <X11/Xlib.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, \
                                #condition); return 1; } \
    ++checks; \
} while (0)

static unsigned long checks;

int main(void)
{
    Display *display = XOpenDisplay(NULL);
    Pixmap pixmap;
    GC gc;
    XImage image;

    CHECK(display != NULL);
    pixmap = XCreatePixmap(display, DefaultRootWindow(display), 1, 1,
                           DefaultDepth(display, DefaultScreen(display)));
    gc = XCreateGC(display, pixmap, 0, NULL);
    CHECK(pixmap != None && gc != NULL);

    memset(&image, 0, sizeof(image));
    image.width = 1;
    image.height = 1;
    image.bytes_per_line = 1;
    image.depth = 1;
    image.format = XYBitmap;
    image.bitmap_pad = 8;

    /* Reject dimensions before XCreateImage or a coordinate-table allocation. */
    _XmPutScaledImage(display, pixmap, gc, &image, 0, 0, 0, 0,
                      1, 1, UINT_MAX, 1);
    _XmPutScaledImage(display, pixmap, gc, &image, 0, 0, 0, 0,
                      UINT_MAX, 1, 1, 1);
    CHECK(XSync(display, False) == 1);

    XFreeGC(display, gc);
    XFreePixmap(display, pixmap);
    XCloseDisplay(display);
    printf("PASS image scaling overflow guards: %lu checks\n", checks);
    return 0;
}
