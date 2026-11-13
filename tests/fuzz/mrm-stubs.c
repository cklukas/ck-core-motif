/*
 * Stand-ins for everything the resource manager (lib/Mrm) calls outside
 * itself: Xlib, the Intrinsics, and the widget library.  The real libMrm
 * sources are compiled unchanged; only the surroundings are replaced.
 *
 * What the stand-ins promise:
 *   - there is no X server.  The Display and Screen are plain structures;
 *     every pixel, pixmap, GC and colour request succeeds with a counter value.
 *   - XCreateImage and XPutImage behave like Xlib for the memory they touch:
 *     XPutImage reads bytes_per_line * height bytes of the image data, so a
 *     loader that hands Xlib too small a buffer is reported by AddressSanitizer
 *     exactly where Xlib would have overread.
 *   - the widget creation functions return a core widget record and the
 *     class records are opaque blocks; the widgets themselves do nothing.
 *   - XtSetValues records the arguments of its last call (fuzz_set_values_args);
 *     unit conversions succeed and keep the value.
 *   - XmCvtByteStreamToXmString reads as many bytes as the stream header
 *     declares, as the real converter does, and returns an opaque block.
 *   - memory comes from malloc; XtMalloc and friends abort on failure like Xt.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlibint.h>
#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <Xm/XmP.h>
#include <Xm/XmI.h>
#include <Xm/ResIndI.h>
#include <Xm/XmosI.h>
#include <Xm/XmRenderTI.h>
#include <Xm/XmTabListI.h>
#include <Xm/Display.h>
#include <Xm/RowColumn.h>

#include "mrm-stubs.h"

static Display *fake_display_ptr;
static Screen fake_screen_rec;
static unsigned long counter = 100;
/* The Xlib macros that lib/Mrm uses (DefaultScreen, DefaultColormap) read the
 * display structure directly, so it is a real one with a single screen. */
static struct _XDisplay display_record;

Display *fuzz_display(void)
{
    if (!fake_display_ptr) {
        fake_display_ptr = (Display *) &display_record;
        display_record.nscreens = 1;
        display_record.default_screen = 0;
        display_record.screens = &fake_screen_rec;
        fake_screen_rec.root_depth = 24;
        fake_screen_rec.cmap = 1;
        fake_screen_rec.black_pixel = 0;
        fake_screen_rec.white_pixel = 0xffffff;
        fake_screen_rec.display = fake_display_ptr;
    }
    return fake_display_ptr;
}

Screen *fuzz_screen(void)
{
    fuzz_display();
    return &fake_screen_rec;
}

/* ---- Xlib ---- */

int XDefaultScreen(Display *d) { (void) d; return 0; }
Screen *XDefaultScreenOfDisplay(Display *d) { (void) d; return fuzz_screen(); }
Colormap XDefaultColormap(Display *d, int s) { (void) d; (void) s; return 1; }
unsigned long XBlackPixelOfScreen(Screen *s) { (void) s; return 0; }
Status XAllocColor(Display *d, Colormap c, XColor *x)
{
    (void) d; (void) c;
    x->pixel = (unsigned long) x->red * 31 + x->green * 7 + x->blue;
    return 1;
}
Status XAllocNamedColor(Display *d, Colormap c, _Xconst char *name, XColor *screen, XColor *exact)
{
    (void) d; (void) c;
    if (!name || !*name || strlen(name) > 40) return 0;
    screen->pixel = (unsigned char) name[0];
    *exact = *screen;
    return 1;
}
XFontSet XCreateFontSet(Display *d, _Xconst char *n, char ***m, int *c, char **def)
{
    (void) d; (void) n;
    *m = NULL; *c = 0; if (def) *def = NULL;
    return NULL;
}
XFontStruct *XLoadQueryFont(Display *d, _Xconst char *n) { (void) d; (void) n; return NULL; }
int XFreeFont(Display *d, XFontStruct *f) { (void) d; (void) f; return 0; }
void XFreeStringList(char **l) { (void) l; }
KeySym XStringToKeysym(_Xconst char *s) { return s && *s ? (KeySym) (unsigned char) *s : NoSymbol; }
GC XCreateGC(Display *d, Drawable w, unsigned long m, XGCValues *v)
{
    (void) d; (void) w; (void) m; (void) v;
    return (GC) malloc(16);
}
int XFreeGC(Display *d, GC g) { (void) d; free(g); return 0; }
Pixmap XCreatePixmap(Display *d, Drawable w, unsigned int wd, unsigned int h, unsigned int depth)
{
    (void) d; (void) w; (void) wd; (void) h; (void) depth;
    return ++counter;
}
int XFreePixmap(Display *d, Pixmap p) { (void) d; (void) p; return 0; }
int XFree(void *p) { free(p); return 0; }

XImage *XCreateImage(Display *d, Visual *v, unsigned int depth, int format, int offset,
                     char *data, unsigned int width, unsigned int height, int pad, int bpl)
{
    (void) d; (void) v;
    if (width == 0 || height == 0 || width > 65535 || height > 65535) return NULL;
    if (pad != 8 && pad != 16 && pad != 32) return NULL;
    XImage *im = calloc(1, sizeof *im);
    im->width = (int) width; im->height = (int) height; im->format = format;
    im->xoffset = offset; im->data = data; im->depth = (int) depth;
    im->byte_order = LSBFirst; im->bitmap_unit = 32; im->bitmap_bit_order = LSBFirst;
    im->bitmap_pad = pad;
    im->bits_per_pixel = format == ZPixmap ? (depth <= 1 ? 1 : depth <= 8 ? 8 : depth <= 16 ? 16 : 32) : 1;
    im->bytes_per_line = bpl;
    /* XInitImage is the function of Xlib that sets the pixel access functions
     * and computes bytes_per_line when it is zero; it needs no display. */
    if (!XInitImage(im)) { free(im); return NULL; }
    return im;
}

int XPutImage(Display *d, Drawable w, GC gc, XImage *im, int sx, int sy, int dx, int dy,
              unsigned int wd, unsigned int h)
{
    (void) d; (void) w; (void) gc; (void) sx; (void) sy; (void) dx; (void) dy; (void) wd; (void) h;
    volatile unsigned char sink = 0;
    size_t rows = (size_t) im->height * (im->format == XYPixmap ? (size_t) im->depth : 1);
    for (size_t i = 0; i < rows * (size_t) im->bytes_per_line; i++)
        sink ^= (unsigned char) im->data[i];
    (void) sink;
    return 0;
}

/* ---- Intrinsics ---- */

char *XtMalloc(Cardinal n) { char *p = malloc(n ? n : 1); if (!p) abort(); return p; }
char *XtCalloc(Cardinal n, Cardinal s) { char *p = calloc(n ? n : 1, s ? s : 1); if (!p) abort(); return p; }
char *XtRealloc(char *p, Cardinal n) { p = realloc(p, n ? n : 1); if (!p) abort(); return p; }
void XtFree(char *p) { free(p); }
void XtWarning(_Xconst char *m) { (void) m; }
char *XtResolvePathname(Display *d, _Xconst char *type, _Xconst char *file, _Xconst char *suffix,
                        _Xconst char *path, Substitution subs, Cardinal n, XtFilePredicate pred)
{
    (void) d; (void) type; (void) file; (void) suffix; (void) path; (void) n; (void) pred;
    /* the path substitution %U is the name given by the caller */
    if (!subs || !subs[0].substitution) return NULL;
    char *r = XtMalloc((Cardinal) strlen(subs[0].substitution) + 1);
    strcpy(r, subs[0].substitution);
    return r;
}
Display *XtDisplayOfObject(Widget w) { (void) w; return fuzz_display(); }
Screen *XtScreenOfObject(Widget w) { (void) w; return fuzz_screen(); }
void XtManageChild(Widget w) { (void) w; }
void XtUnmanageChild(Widget w) { (void) w; }
/* The last XtSetValues call, for the tests that check what MrmFetchSetValues hands to the widget. */
Cardinal fuzz_set_values_count;
Arg fuzz_set_values_args[FUZZ_SET_VALUES_MAX];
void XtSetValues(Widget w, ArgList a, Cardinal n)
{
    (void) w;
    fuzz_set_values_count = n < FUZZ_SET_VALUES_MAX ? n : FUZZ_SET_VALUES_MAX;
    for (Cardinal i = 0; i < fuzz_set_values_count; i++) fuzz_set_values_args[i] = a[i];
}
void XtGetValues(Widget w, ArgList a, Cardinal n)
{
    (void) w;
    for (Cardinal i = 0; i < n; i++)
        if (a[i].value) *(XtArgVal *) a[i].value = 0;
}
void XtAddCallback(Widget w, _Xconst char *n, XtCallbackProc p, XtPointer c)
{ (void) w; (void) n; (void) p; (void) c; }
Widget XtNameToWidget(Widget w, _Xconst char *n) { (void) w; (void) n; return NULL; }
XtTranslations XtParseTranslationTable(_Xconst char *s) { (void) s; return (XtTranslations) malloc(8); }
XtAccelerators XtParseAcceleratorTable(_Xconst char *s) { (void) s; return (XtAccelerators) malloc(8); }

/* ---- widget library ---- */

static WidgetRec **widgets;
static size_t widget_count, widget_cap;

void fuzz_free_widgets(void)
{
    for (size_t i = 0; i < widget_count; i++) free(widgets[i]);
    widget_count = 0;
}

Widget fuzz_new_widget(Widget parent)
{
    WidgetRec *w = calloc(1, sizeof *w);
    if (widget_count == widget_cap) {
        widget_cap = widget_cap ? widget_cap * 2 : 64;
        widgets = realloc(widgets, widget_cap * sizeof *widgets);
    }
    widgets[widget_count++] = w;
    static WidgetClassRec class_record;
    class_record.core_class.class_inited = 0xff;     /* a widget of every kind */
    w->core.widget_class = &class_record;
    w->core.self = (Widget) w;
    w->core.parent = parent;
    w->core.screen = fuzz_screen();
    w->core.depth = 24;
    w->core.colormap = 1;
    return (Widget) w;
}

Display *_XmGetDefaultDisplay(void) { return fuzz_display(); }
Widget XmGetXmDisplay(Display *d) { (void) d; return fuzz_new_widget(NULL); }
Widget XmGetTearOffControl(Widget m) { (void) m; return NULL; }
Boolean XmeNamesAreEqual(char *a, char *b) { return a && b && strcmp(a, b) == 0; }
String _XmOSInitPath(String file, String env, Boolean *user_path)
{
    (void) file; (void) env;
    *user_path = False;
    /* the caller owns the string and releases it with XtFree */
    char *path = XtMalloc(3);
    strcpy(path, "%U");
    return path;
}
int _XmConvertUnits(Screen *s, int dim, int from_type, int from_val, int to_type)
{ (void) s; (void) dim; (void) from_type; (void) to_type; return from_val; }
int _XmConvertFloatUnitsToIntUnits(int type, float v, int *itype, float *ival, int dflt)
{ (void) dflt; *itype = type; *ival = v; return 1; }
Pixmap XmGetPixmapByDepth(Screen *s, char *n, Pixel fg, Pixel bg, int depth)
{ (void) s; (void) n; (void) fg; (void) bg; (void) depth; return ++counter; }

static void *block(size_t n) { return calloc(1, n); }

XmString XmStringCopy(XmString s) { (void) s; return (XmString) block(16); }
void XmStringFree(XmString s) { free(s); }
XmString XmStringGenerate(XtPointer text, XmStringTag tag, XmTextType type, XmStringTag rend)
{
    (void) tag; (void) type; (void) rend;
    /* the text must be a valid string: the real function measures it */
    if (text) (void) strlen((char *) text);
    return (XmString) block(16);
}
XmString XmCvtByteStreamToXmString(unsigned char *p)
{
    /* ASN.1 header: 0xdf 0x80 0x06 then a short or long form length */
    size_t len = 0, off = 3;
    if (p[0] != 0xdf || p[1] != 0x80 || p[2] != 0x06) return NULL;
    if (p[3] < 0x80) { len = p[3]; off = 4; }
    else {
        unsigned n = p[3] & 0x7f;
        if (n == 0 || n > 4) return NULL;
        for (unsigned i = 0; i < n; i++) len = (len << 8) | p[4 + i];
        off = 4 + n;
    }
    volatile unsigned char sink = 0;
    for (size_t i = 0; i < len; i++) sink ^= p[off + i];
    (void) sink;
    return (XmString) block(16);
}
unsigned int XmCvtXmStringToByteStream(XmString s, unsigned char **out) { (void) s; *out = NULL; return 0; }
XmFontList XmFontListCreate(XFontStruct *f, XmStringCharSet c) { (void) f; (void) c; return (XmFontList) block(16); }
XmFontList XmFontListAdd(XmFontList o, XFontStruct *f, XmStringCharSet c) { (void) f; (void) c; return o ? o : (XmFontList) block(16); }
XmFontListEntry XmFontListEntryCreate(char *t, XmFontType ty, XtPointer f) { (void) t; (void) ty; (void) f; return (XmFontListEntry) block(16); }
void XmFontListEntryFree(XmFontListEntry *e) { if (e) { free(*e); *e = NULL; } }
XmFontList XmFontListAppendEntry(XmFontList o, XmFontListEntry e) { (void) e; return o ? o : (XmFontList) block(16); }
void XmFontListFree(XmFontList l) { free(l); }
void XmRenderTableFree(XmRenderTable t) { free(t); }
void XmRenditionFree(XmRendition r) { free(r); }
Widget _XmCreateRenderTable(Widget p, String n, ArgList a, Cardinal c) { (void) p; (void) n; (void) a; (void) c; return NULL; }
Widget _XmCreateRendition(Widget p, String n, ArgList a, Cardinal c) { (void) p; (void) n; (void) a; (void) c; return NULL; }
Widget _XmCreateTab(Widget p, String n, ArgList a, Cardinal c) { (void) p; (void) n; (void) a; (void) c; return NULL; }
Widget _XmCreateTabList(Widget p, String n, ArgList a, Cardinal c) { (void) p; (void) n; (void) a; (void) c; return NULL; }
Display *_XmRenderTableDisplay(XmRenderTable t) { (void) t; return fuzz_display(); }
