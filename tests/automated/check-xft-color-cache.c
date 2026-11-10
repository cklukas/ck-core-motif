/*
 * The Xft colour cache must key its entries by colormap and pixel, belong to
 * one display, go away with it, stay bounded, and never serve a colormap whose
 * cells can change.
 *
 *   check-xft-color-cache
 *
 * Needs two X servers: $DISPLAY with a TrueColor default visual and
 * $DISPLAY2 with an 8-bit PseudoColor one.
 *
 * Define XFT_CACHE_OLD_API to build the program against the earlier toolkit,
 * whose cache was one process-wide array keyed by the pixel alone and took no
 * colormap.
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

typedef unsigned long Pixel;

#ifdef XFT_CACHE_OLD_API
extern XftColor _XmXftGetXftColor(Display *, Pixel);
#define GET_COLOR(d, cmap, pixel) _XmXftGetXftColor((d), (pixel))
#else
extern XftColor _XmXftGetXftColor(Display *, Colormap, Pixel);
extern unsigned int _XmXftColorCacheCount(Display *);
#define GET_COLOR(d, cmap, pixel) _XmXftGetXftColor((d), (cmap), (pixel))
#endif

static int query_calls;

int XQueryColor(Display * d, Colormap cmap, XColor * color)
{
    static int (*real)(Display *, Colormap, XColor *);
    query_calls++;
    if (!real) real = dlsym(RTLD_NEXT, "XQueryColor");
    return real(d, cmap, color);
}

static int failures;
#define CHECK(cond, ...) \
    do { \
	if (!(cond)) { \
	    failures++; \
	    fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__); \
	    fprintf(stderr, __VA_ARGS__); \
	    fprintf(stderr, "\n"); \
	} \
    } while (0)

static int ignore_error(Display * d, XErrorEvent * e) { return 0; }

static XColor truth(Display * d, Colormap cmap, Pixel pixel)
{
    XColor c;
    memset(&c, 0, sizeof(c));
    c.pixel = pixel;
    XQueryColor(d, cmap, &c);
    return c;
}

static int same(XftColor got, XColor want)
{
    return got.color.red == want.red && got.color.green == want.green &&
	   got.color.blue == want.blue;
}

int main(void)
{
    Display * a = XOpenDisplay(getenv("DISPLAY"));
    Display * b = XOpenDisplay(getenv("DISPLAY2"));
    Colormap cmap_a, cmap_b;
    Pixel p;
    XColor want, red;
    XftColor got;
    int calls;

    if (!a || !b) {
	fprintf(stderr, "needs DISPLAY and DISPLAY2\n");
	return 2;
    }
    XSetErrorHandler(ignore_error);
    cmap_a = DefaultColormap(a, DefaultScreen(a));
    cmap_b = DefaultColormap(b, DefaultScreen(b));
    CHECK(DefaultVisual(a, 0)->class == TrueColor, "display A must be TrueColor");
    CHECK(DefaultVisual(b, 0)->class == PseudoColor, "display B must be PseudoColor");

    /* The same pixel is asked for again: the server is not. */
    p = 0x336699;
    want = truth(a, cmap_a, p);
    got = GET_COLOR(a, cmap_a, p);
    CHECK(same(got, want), "colour of a TrueColor pixel: %04x %04x %04x vs %04x %04x %04x",
	  got.color.red, got.color.green, got.color.blue, want.red, want.green, want.blue);
    calls = query_calls;
    got = GET_COLOR(a, cmap_a, p);
    CHECK(same(got, want) && query_calls == calls,
	  "repeating a lookup must come from the cache (%d server queries)", query_calls - calls);
#ifndef XFT_CACHE_OLD_API
    CHECK(_XmXftColorCacheCount(a) == 1, "one colour cached for display A (%u)",
	  _XmXftColorCacheCount(a));
#endif

    /* Another display has its own colours for the same pixel number. */
    {
	Visual * vb = DefaultVisual(b, DefaultScreen(b));
	XColor cell;
	(void) vb;
	memset(&cell, 0, sizeof(cell));
	cell.red = 0xffff;
	if (!XAllocColor(b, cmap_b, &cell)) {
	    fprintf(stderr, "cannot allocate a colour on display B\n");
	    return 2;
	}
	red = cell;
    }
    /* Make the cache hold that very pixel number for display A first. */
    (void) GET_COLOR(a, cmap_a, red.pixel);
    got = GET_COLOR(b, cmap_b, red.pixel);
    want = truth(b, cmap_b, red.pixel);
    CHECK(same(got, want) && got.color.red == 0xffff,
	  "pixel %lu on display B is red, got %04x %04x %04x", red.pixel,
	  got.color.red, got.color.green, got.color.blue);

#ifndef XFT_CACHE_OLD_API
    /* A colormap whose cells can change is never cached. */
    {
	Colormap own = XCreateColormap(b, RootWindow(b, DefaultScreen(b)),
				       DefaultVisual(b, DefaultScreen(b)), AllocNone);
	unsigned long cell, planes[1];
	XColor c;
	unsigned int before = _XmXftColorCacheCount(b);
	if (XAllocColorCells(b, own, False, planes, 0, &cell, 1)) {
	    memset(&c, 0, sizeof(c));
	    c.pixel = cell; c.red = 0xffff; c.flags = DoRed | DoGreen | DoBlue;
	    XStoreColor(b, own, &c);
	    got = GET_COLOR(b, own, cell);
	    CHECK(got.color.red == 0xffff && got.color.blue == 0, "writable cell is red");
	    c.red = 0; c.blue = 0xffff;
	    XStoreColor(b, own, &c);
	    got = GET_COLOR(b, own, cell);
	    CHECK(got.color.red == 0 && got.color.blue == 0xffff,
		  "writable cell was changed to blue, got %04x %04x %04x",
		  got.color.red, got.color.green, got.color.blue);
	    CHECK(_XmXftColorCacheCount(b) == before, "writable colormaps are not cached");
	} else
	    fprintf(stderr, "note: no writable cells on display B\n");
	/* A private colormap of the TrueColor display is not cached either. */
	{
	    Colormap private_a = XCreateColormap(a, RootWindow(a, DefaultScreen(a)),
						 DefaultVisual(a, DefaultScreen(a)), AllocNone);
	    unsigned int count = _XmXftColorCacheCount(a);
	    (void) GET_COLOR(a, private_a, 0x102030);
	    CHECK(_XmXftColorCacheCount(a) == count, "a colormap other than the default is not cached");
	}
    }
#endif

    /* The table is bounded and its answers stay right. */
    {
	Pixel k;
	int bad = 0;
	for (k = 1000; k < 1400; k++) {
	    XColor w = truth(a, cmap_a, k * 257);
	    XftColor g = GET_COLOR(a, cmap_a, k * 257);
	    if (!same(g, w)) bad++;
	}
	CHECK(bad == 0, "%d wrong colours while the cache cycled", bad);
#ifndef XFT_CACHE_OLD_API
	CHECK(_XmXftColorCacheCount(a) <= 128, "the cache holds %u colours", _XmXftColorCacheCount(a));
#endif
    }

#ifndef XFT_CACHE_OLD_API
    /* The cache goes away with its display. */
    CHECK(_XmXftColorCacheCount(a) > 0, "display A has cached colours");
    XCloseDisplay(a);
    CHECK(_XmXftColorCacheCount(NULL) < 128 + 1 && _XmXftColorCacheCount(b) <= 1,
	  "unexpected colours cached for display B");
    {
	unsigned int total = _XmXftColorCacheCount(NULL);
	CHECK(total == _XmXftColorCacheCount(b), "closing display A must release its colours (%u left, %u for B)",
	      total, _XmXftColorCacheCount(b));
    }
    XCloseDisplay(b);
    CHECK(_XmXftColorCacheCount(NULL) == 0, "no colours may remain after both displays are closed (%u)",
	  _XmXftColorCacheCount(NULL));
#endif

    printf("xft color cache: %s\n", failures ? "FAILED" : "ok");
    return failures != 0;
}
