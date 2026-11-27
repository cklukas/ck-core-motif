/*
 * Fuzz target: ImageCache & ReadImage XBM bitmap / XPM symbolic-color / HiDPI
 * scaled-image pipeline (lib/Xm/ImageCache.c, lib/Xm/ReadImage.c, lib/Xm/Hash.c,
 * lib/Xm/Xpm*.c, compiled unchanged).
 */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define XLIB_ILLEGAL_ACCESS 1
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/BaseClassP.h>
#include <Xm/AccColorT.h>
#include <Xm/ColorObjP.h>
#include <Xm/IconFile.h>
#include "ImageCachI.h"
#include "ReadImageI.h"

/* ---- Headless Display / Screen / Visual stand-in for ImageCache & XCreateImage ---- */

static Visual g_visual;
static Depth g_depths[2];
static Screen g_screen;
static ScreenFormat g_formats[3];
static struct _XDisplay g_display;
static int g_inited;
static char g_scratch_path[512];

static Screen *headless_screen(void)
{
    if (!g_inited) {
        memset(&g_visual, 0, sizeof(g_visual));
        g_visual.visualid = 0x21;
        g_visual.class = TrueColor;
        g_visual.red_mask = 0x00ff0000UL;
        g_visual.green_mask = 0x0000ff00UL;
        g_visual.blue_mask = 0x000000ffUL;
        g_visual.bits_per_rgb = 8;
        g_visual.map_entries = 256;

        g_formats[0].depth = 1;
        g_formats[0].bits_per_pixel = 1;
        g_formats[0].scanline_pad = 32;
        g_formats[1].depth = 8;
        g_formats[1].bits_per_pixel = 8;
        g_formats[1].scanline_pad = 32;
        g_formats[2].depth = 24;
        g_formats[2].bits_per_pixel = 32;
        g_formats[2].scanline_pad = 32;

        g_depths[0].depth = 1;
        g_depths[0].nvisuals = 0;
        g_depths[1].depth = 24;
        g_depths[1].nvisuals = 1;
        g_depths[1].visuals = &g_visual;

        memset(&g_screen, 0, sizeof(g_screen));
        g_screen.display = &g_display;
        g_screen.root = (Window) 1;
        g_screen.width = 1024;
        g_screen.height = 768;
        g_screen.mwidth = 270;
        g_screen.mheight = 203;
        g_screen.ndepths = 2;
        g_screen.depths = g_depths;
        g_screen.root_depth = 24;
        g_screen.root_visual = &g_visual;
        g_screen.cmap = (Colormap) 1;
        g_screen.white_pixel = 0x00ffffffUL;
        g_screen.black_pixel = 0x00000000UL;

        memset(&g_display, 0, sizeof(g_display));
        g_display.byte_order = LSBFirst;
        g_display.bitmap_unit = 32;
        g_display.bitmap_pad = 32;
        g_display.bitmap_bit_order = LSBFirst;
        g_display.nformats = 3;
        g_display.pixmap_format = g_formats;
        g_display.default_screen = 0;
        g_display.nscreens = 1;
        g_display.screens = &g_screen;

        const char *td = getenv("TMPDIR");
        if (!td || !td[0])
            td = "/var/tmp";
        snprintf(g_scratch_path, sizeof(g_scratch_path), "%s/fuzz-imgcache-%ld.img",
                 td, (long) getpid());
        g_inited = 1;
    }
    return &g_screen;
}

/* ---- Xlib / Motif stand-ins for headless ImageCache execution ---- */

static Pixmap g_next_pixmap = 100;
static struct _XGC *g_dummy_gc = (struct _XGC *) (uintptr_t) 0x10;

Display *_XmGetDefaultDisplay(void) { return &g_display; }
Widget XmGetXmDisplay(Display *d) { (void) d; return NULL; }
XtAppContext XtDisplayToApplicationContext(Display *d) { (void) d; return NULL; }
XtAppContext XtWidgetToApplicationContext(Widget w) { (void) w; return NULL; }
Display *XtDisplayOfObject(Widget w) { (void) w; return &g_display; }
Screen *XtScreenOfObject(Widget w) { (void) w; return headless_screen(); }
XtPointer XmeTraitGet(XtPointer o, XrmQuark q) { (void) o; (void) q; return NULL; }
XrmQuark XmQmotif;
XmGenericClassExt *_XmGetClassExtensionPtr(XmGenericClassExt *p, XrmQuark q) { (void) p; (void) q; return NULL; }
double _XmGetWidgetDpiScale(Widget w) { (void) w; return 1.0; }
void XmeWarning(Widget w, char *m) { (void) w; (void) m; }

String XmGetIconFileName(Screen *screen, String imageInstanceName, String imageName,
                         String host_name, unsigned int size)
{
    (void) screen; (void) imageInstanceName; (void) host_name; (void) size;
    if (!imageName)
        return NULL;
    return XtNewString(imageName);
}

Boolean XmeGetIconControlInfo(Screen *screen, Boolean *use_mask_rtn,
                              Boolean *use_color_rtn, Boolean *use_icon_file_cache_rtn)
{
    (void) screen;
    if (use_mask_rtn) *use_mask_rtn = True;
    if (use_color_rtn) *use_color_rtn = True;
    if (use_icon_file_cache_rtn) *use_icon_file_cache_rtn = False;
    return True;
}

Boolean XmeGetColorObjData(Screen *screen, int *colorUse, XmPixelSet *pixelSets,
                           unsigned short num_pixel_sets, short *active, short *inactive,
                           short *primary, short *secondary, short *text)
{
    (void) screen; (void) colorUse; (void) pixelSets; (void) num_pixel_sets;
    (void) active; (void) inactive; (void) primary; (void) secondary; (void) text;
    return False;
}

Boolean _XmSearchColorCache(unsigned int which, XmColorData *values, XmColorData **ret)
{
    (void) which; (void) values; (void) ret;
    return False;
}

void XmGetColors(Screen *s, Colormap c, Pixel bg, Pixel *fg, Pixel *ts, Pixel *bs, Pixel *sc)
{
    (void) s; (void) c;
    if (fg) *fg = ~bg & 0x00ffffffUL;
    if (ts) *ts = 0x00ddddddUL;
    if (bs) *bs = 0x00444444UL;
    if (sc) *sc = 0x00888888UL;
}

void _XmOSGenerateMaskName(String imageName, String maskNameBuf)
{
    if (!imageName || !maskNameBuf)
        return;
    size_t n = strlen(imageName);
    memcpy(maskNameBuf, imageName, n);
    memcpy(maskNameBuf + n, "_m", 3);
}

double _XmGetDisplayScaleFactor(Display *dpy)
{
    (void) dpy;
    return 1.0;
}

Status XParseColor(Display *d, Colormap cmap, _Xconst char *spec, XColor *exact)
{
    (void) d; (void) cmap;
    if (!spec || !spec[0] || !exact)
        return 0;
    unsigned int r = 0, g = 0, b = 0;
    if (spec[0] == '#' && strlen(spec) == 7 &&
        sscanf(spec + 1, "%02x%02x%02x", &r, &g, &b) == 3) {
        exact->red = (unsigned short) (r * 257U);
        exact->green = (unsigned short) (g * 257U);
        exact->blue = (unsigned short) (b * 257U);
        exact->flags = DoRed | DoGreen | DoBlue;
        return 1;
    }
    if (strcmp(spec, "None") == 0 || strcmp(spec, "none") == 0)
        return 0;
    unsigned int h = 2166136261U;
    for (const unsigned char *p = (const unsigned char *) spec; *p; ++p)
        h = (h ^ *p) * 16777619U;
    exact->red = (unsigned short) ((h & 0xffU) * 257U);
    exact->green = (unsigned short) (((h >> 8) & 0xffU) * 257U);
    exact->blue = (unsigned short) (((h >> 16) & 0xffU) * 257U);
    exact->flags = DoRed | DoGreen | DoBlue;
    return 1;
}

Status XAllocColor(Display *d, Colormap cmap, XColor *in_out)
{
    (void) d; (void) cmap;
    if (!in_out)
        return 0;
    in_out->pixel = (((unsigned long) (in_out->red >> 8)) << 16) |
                    (((unsigned long) (in_out->green >> 8)) << 8) |
                    ((unsigned long) (in_out->blue >> 8));
    return 1;
}

int XQueryColor(Display *d, Colormap cmap, XColor *def_in_out)
{
    (void) d; (void) cmap;
    if (def_in_out) {
        def_in_out->red = (unsigned short) (((def_in_out->pixel >> 16) & 0xffUL) * 257UL);
        def_in_out->green = (unsigned short) (((def_in_out->pixel >> 8) & 0xffUL) * 257UL);
        def_in_out->blue = (unsigned short) ((def_in_out->pixel & 0xffUL) * 257UL);
    }
    return 1;
}

int XQueryColors(Display *d, Colormap cmap, XColor *defs, int ncolors)
{
    for (int i = 0; i < ncolors; ++i)
        (void) XQueryColor(d, cmap, &defs[i]);
    return 1;
}

int XFreeColors(Display *d, Colormap cmap, unsigned long *pixels, int npixels, unsigned long planes)
{
    (void) d; (void) cmap; (void) pixels; (void) npixels; (void) planes;
    return 1;
}

Pixmap XCreatePixmap(Display *d, Drawable dr, unsigned int w, unsigned int h, unsigned int depth)
{
    (void) d; (void) dr;
    if (w == 0 || h == 0 || w > 1024 || h > 1024 || depth == 0)
        return None;
    return ++g_next_pixmap;
}

int XFreePixmap(Display *d, Pixmap p) { (void) d; (void) p; return 1; }
GC XCreateGC(Display *d, Drawable dr, unsigned long m, XGCValues *v)
{ (void) d; (void) dr; (void) m; (void) v; return g_dummy_gc; }
int XFreeGC(Display *d, GC gc) { (void) d; (void) gc; return 1; }
int XSetForeground(Display *d, GC gc, unsigned long fg) { (void) d; (void) gc; (void) fg; return 1; }
int XSetBackground(Display *d, GC gc, unsigned long bg) { (void) d; (void) gc; (void) bg; return 1; }
int XCopyArea(Display *d, Drawable s, Drawable dst, GC g, int sx, int sy,
              unsigned int w, unsigned int h, int dx, int dy)
{ (void) d; (void) s; (void) dst; (void) g; (void) sx; (void) sy; (void) w; (void) h; (void) dx; (void) dy; return 1; }
int XCopyPlane(Display *d, Drawable s, Drawable dst, GC g, int sx, int sy,
               unsigned int w, unsigned int h, int dx, int dy, unsigned long plane)
{ (void) d; (void) s; (void) dst; (void) g; (void) sx; (void) sy; (void) w; (void) h; (void) dx; (void) dy; (void) plane; return 1; }

Status XGetGeometry(Display *d, Drawable dr, Window *root_return,
                    int *x_return, int *y_return,
                    unsigned int *width_return, unsigned int *height_return,
                    unsigned int *border_width_return, unsigned int *depth_return)
{
    (void) d; (void) dr;
    if (root_return) *root_return = g_screen.root;
    if (x_return) *x_return = 0;
    if (y_return) *y_return = 0;
    if (width_return) *width_return = 256;
    if (height_return) *height_return = 256;
    if (border_width_return) *border_width_return = 0;
    if (depth_return) *depth_return = 24;
    return 1;
}

int XPutImage(Display *d, Drawable dr, GC gc, XImage *image,
              int src_x, int src_y, int dest_x, int dest_y,
              unsigned int width, unsigned int height)
{
    (void) d; (void) dr; (void) gc; (void) dest_x; (void) dest_y;
    if (image && image->data && width > 0 && height > 0 &&
        src_x >= 0 && src_y >= 0 &&
        (unsigned int) src_x + width <= (unsigned int) image->width &&
        (unsigned int) src_y + height <= (unsigned int) image->height &&
        width <= 256 && height <= 256) {
        /* Touch first and last pixel to ensure ASan verifies backing buffer bounds */
        (void) XGetPixel(image, src_x, src_y);
        (void) XGetPixel(image, src_x + (int) width - 1, src_y + (int) height - 1);
    }
    return 1;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size == 0 || size > 16384)
        return 0;

    Screen *scr = headless_screen();
    FILE *fp = fopen(g_scratch_path, "wb");
    if (!fp)
        return 0;
    (void) fwrite(data, 1, size, fp);
    fclose(fp);

    /* 1. Direct ReadImage XBM bitmap + hotspot parser */
    int hot_x = -1, hot_y = -1;
    XImage *xbm = _XmReadImageAndHotSpotFromFile(&g_display, g_scratch_path, &hot_x, &hot_y);
    if (xbm) {
        if (xbm->width > 0 && xbm->height > 0 && xbm->width <= 128 && xbm->height <= 128) {
            _XmPutScaledImage(&g_display, (Drawable) 2, g_dummy_gc, xbm,
                              0, 0, 0, 0,
                              (unsigned int) xbm->width, (unsigned int) xbm->height,
                              (unsigned int) xbm->width * 2U, (unsigned int) xbm->height * 2U);
        }
        XDestroyImage(xbm);
    }

    /* 2. Full ImageCache + symbolic XPM / XBM + HiDPI scaled pixmap lookup & eviction */
    XmAccessColorDataRec acc = {
        .background = 0x00eef2f6UL,
        .foreground = 0x00112233UL,
        .top_shadow_color = 0x00ffffffUL,
        .bottom_shadow_color = 0x00222222UL,
        .select_color = 0x008899aaUL,
        .highlight_color = 0x00ff8800UL,
    };
    Pixmap p1 = _XmGetScaledPixmap(scr, NULL, g_scratch_path, &acc, 24, False, 1.5);
    if (p1 != XmUNSPECIFIED_PIXMAP && p1 != None)
        XmDestroyPixmap(scr, p1);
    Pixmap p2 = XmGetPixmapByDepth(scr, g_scratch_path, 0x00112233UL, 0x00eef2f6UL, 24);
    if (p2 != XmUNSPECIFIED_PIXMAP && p2 != None)
        XmDestroyPixmap(scr, p2);

    XImage *cached = NULL;
    if (_XmGetImage(scr, g_scratch_path, &cached) && cached) {
        XmUninstallImage(cached);
    }
    char mask_name[540];
    _XmOSGenerateMaskName(g_scratch_path, mask_name);
    XImage *mask_cached = NULL;
    if (_XmInImageCache(mask_name) && _XmGetImage(scr, mask_name, &mask_cached) && mask_cached) {
        XmUninstallImage(mask_cached);
    }

    unlink(g_scratch_path);
    return 0;
}
