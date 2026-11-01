/* Native scaling oracle and malformed-image corpus for _XmPutScaledImage. */
#define _GNU_SOURCE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

/* Internal entry point; use this ABI declaration to keep the probe independent
 * of generated Motif headers. */
extern void _XmPutScaledImage(Display *, Drawable, GC, XImage *, int, int,
                              int, int, unsigned int, unsigned int,
                              unsigned int, unsigned int);

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "line %d: %s\n", __LINE__, #c); exit(1); } } while (0)
static int errors;
static int x_error(Display *display, XErrorEvent *event)
{ (void)display; (void)event; ++errors; return 0; }

static void unchanged(Display *display, Pixmap pixmap, unsigned long expected)
{
    XImage *actual;
    XSync(display, False);
    CHECK(errors == 0);
    actual = XGetImage(display, pixmap, 0, 0, 4, 4, AllPlanes, ZPixmap);
    CHECK(actual != NULL);
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            CHECK(XGetPixel(actual, x, y) == expected);
    XDestroyImage(actual);
}

int main(void)
{
    Dl_info loaded;
    char *actual_library, *expected_library;
    const char *expected = getenv("CKMOTIF_EXPECT_LIBRARY");
    Display *display = XOpenDisplay(NULL);
    XImage *image, *actual;
    XImage malformed;
    Pixmap pixmap;
    GC gc;
    int screen;
    unsigned long black, white;
    CHECK(expected);
    CHECK(dladdr((void *)_XmPutScaledImage, &loaded) != 0);
    actual_library = realpath(loaded.dli_fname, NULL);
    expected_library = realpath(expected, NULL);
    CHECK(actual_library && expected_library);
    CHECK(strcmp(actual_library, expected_library) == 0);
    printf("Loaded library: %s\n", actual_library);
    free(actual_library); free(expected_library);
    CHECK(display != NULL);
    XSetErrorHandler(x_error);
    screen = DefaultScreen(display);
    black = BlackPixel(display, screen); white = WhitePixel(display, screen);
    pixmap = XCreatePixmap(display, RootWindow(display, screen), 4, 4,
                           DefaultDepth(display, screen));
    gc = XCreateGC(display, pixmap, 0, NULL);
    image = XCreateImage(display, DefaultVisual(display, screen),
                         DefaultDepth(display, screen), ZPixmap, 0, NULL,
                         2, 2, 32, 0);
    CHECK(image && gc && image->bytes_per_line > 0);
    image->data = calloc(2, (size_t)image->bytes_per_line);
    CHECK(image->data);
    XPutPixel(image, 0, 0, black); XPutPixel(image, 1, 0, white);
    XPutPixel(image, 0, 1, white); XPutPixel(image, 1, 1, black);
    XSetForeground(display, gc, black);
    XFillRectangle(display, pixmap, gc, 0, 0, 4, 4);

    /* Begin from a real initialized, non-null-data XImage. Each malformed
     * case changes one field so it reaches its intended validation branch. */
#define REJECT_FIELD(field, value) do { \
    malformed = *image; malformed.field = (value); \
    _XmPutScaledImage(display,pixmap,gc,&malformed,0,0,0,0,2,2,4,4); \
    unchanged(display,pixmap,black); \
} while (0)
    REJECT_FIELD(data, NULL);
    REJECT_FIELD(width, 0);
    REJECT_FIELD(height, -1);
    REJECT_FIELD(bytes_per_line, 0);
    REJECT_FIELD(bytes_per_line, 1);
    REJECT_FIELD(bits_per_pixel, 0);
    REJECT_FIELD(depth, 0);
    REJECT_FIELD(format, -1);
#define REJECT_RECT(sx, sy, dx, dy, sw, sh, dw, dh) do { \
    _XmPutScaledImage(display,pixmap,gc,image,sx,sy,dx,dy,sw,sh,dw,dh); \
    unchanged(display,pixmap,black); \
} while (0)
    REJECT_RECT(-1,0,0,0,2,2,4,4);
    REJECT_RECT(0,-1,0,0,2,2,4,4);
    REJECT_RECT(1,0,0,0,2,2,4,4);
    REJECT_RECT(0,1,0,0,2,2,4,4);
    REJECT_RECT(0,0,0,0,0,2,4,4);
    REJECT_RECT(0,0,0,0,2,2,UINT_MAX,4);
    REJECT_RECT(0,0,0,0,2,2,4,UINT_MAX);
    REJECT_RECT(0,0,INT_MAX,0,2,2,4,4);
    REJECT_RECT(0,0,0,INT_MAX,2,2,4,4);

    /* A positive oracle ensures that an unconditional early return cannot
     * make the malformed-input corpus appear to pass. */
    _XmPutScaledImage(display,pixmap,gc,image,0,0,0,0,2,2,4,4);
    XSync(display,False); CHECK(errors == 0);
    actual = XGetImage(display,pixmap,0,0,4,4,AllPlanes,ZPixmap);
    CHECK(actual);
    for (int y=0; y<4; ++y)
        for (int x=0; x<4; ++x)
            CHECK(XGetPixel(actual,x,y) == (((x/2) == (y/2)) ? black : white));
    XDestroyImage(actual);
    XDestroyImage(image); XFreeGC(display,gc); XFreePixmap(display,pixmap);
    XCloseDisplay(display);
    puts("PASS 17 malformed scaling cases and actual 2x pixel scaling");
    return 0;
}
