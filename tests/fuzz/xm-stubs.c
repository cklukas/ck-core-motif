/*
 * Stand-ins for what the compound string, render table, tab list and resource
 * converter sources (lib/Xm/XmString.c, XmRenderT.c, XmTabList.c, Direction.c,
 * Xmos.c, ResEncod.c, ResInd.c, ResConvert.c) call outside themselves.  Those
 * sources are compiled unchanged.
 *
 *   - There is no X server and no widget tree.  The Display and Screen are
 *     plain blocks, fonts never load, atoms are numbered, nothing is drawn.
 *     Drawing and text measuring calls abort: if a harness reaches one, the
 *     harness is wrong, not the code under test.
 *   - Traits, representation types and widget class extensions are absent
 *     (lookups return nothing), which is what a converter sees when the
 *     widget in question does not provide them.
 *   - Warnings and Xt error messages are counted and dropped.
 *   - Everything else that Xt and Xlib provide without a display (memory,
 *     quarks, keysym names, converter registration) is the real libXt and libX11.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/BaseClassP.h>
#include <Xm/RepType.h>

#define UNREACHABLE(name) do { fprintf(stderr, "xm-stubs: %s called\n", name); abort(); } while (0)

unsigned long fuzz_warnings;

static char display_block[1024];
static Screen screen_block;

Display *fuzz_display(void)
{
    screen_block.display = (Display *) display_block;
    /* a 1024 x 768 pixel screen of 270 x 203 mm, 24 bits deep */
    screen_block.width = 1024;
    screen_block.height = 768;
    screen_block.mwidth = 270;
    screen_block.mheight = 203;
    screen_block.root_depth = 24;
    return (Display *) display_block;
}

/* ---- Xlib: no server ---- */

int XChangeGC(Display *d, GC g, unsigned long m, XGCValues *v) { (void) d; (void) g; (void) m; (void) v; UNREACHABLE("XChangeGC"); }
int XSetClipMask(Display *d, GC g, Pixmap p) { (void) d; (void) g; (void) p; UNREACHABLE("XSetClipMask"); }
int XSetClipRectangles(Display *d, GC g, int x, int y, XRectangle *r, int n, int o)
{ (void) d; (void) g; (void) x; (void) y; (void) r; (void) n; (void) o; UNREACHABLE("XSetClipRectangles"); }
Status XGetGCValues(Display *d, GC g, unsigned long m, XGCValues *v) { (void) d; (void) g; (void) m; (void) v; UNREACHABLE("XGetGCValues"); }
Status XGetWindowAttributes(Display *d, Window w, XWindowAttributes *a) { (void) d; (void) w; (void) a; return 0; }
Atom XInternAtom(Display *d, _Xconst char *name, Bool only)
{
    (void) d; (void) only;
    return name ? (Atom) (1 + (unsigned char) name[0]) : None;
}
Status XInternAtoms(Display *d, char **names, int n, Bool only, Atom *out)
{
    (void) d; (void) only;
    for (int i = 0; i < n; i++) out[i] = names[i] ? (Atom) (1 + (unsigned char) names[i][0]) : None;
    return 1;
}
int XmbTextPropertyToTextList(Display *d, const XTextProperty *tp, char ***list_return, int *count_return)
{
    (void) d;
    if (list_return) *list_return = NULL;
    if (count_return) *count_return = 0;
    if (tp && tp->value && tp->nitems > 0) {
        volatile unsigned char sink = 0;
        for (unsigned long i = 0; i < tp->nitems; ++i)
            sink ^= tp->value[i];
        (void) sink;
    }
    return XConverterNotFound;
}
int XmbTextListToTextProperty(Display *d, char **list, int count, XICCEncodingStyle style, XTextProperty *text_prop_return)
{
    (void) d; (void) style;
    if (text_prop_return)
        memset(text_prop_return, 0, sizeof(*text_prop_return));
    for (int i = 0; list && i < count; ++i) {
        if (list[i])
            (void) strlen(list[i]);
    }
    return XConverterNotFound;
}
/* ---- Intrinsics: calls that need a display or a widget ---- */

void XtWarning(_Xconst char *m) { (void) m; fuzz_warnings++; }
void XtAppWarningMsg(XtAppContext a, _Xconst char *n, _Xconst char *t, _Xconst char *c, _Xconst char *d,
                     String *p, Cardinal *np)
{ (void) a; (void) n; (void) t; (void) c; (void) d; (void) p; (void) np; fuzz_warnings++; }
void XtWarningMsg(_Xconst char *n, _Xconst char *t, _Xconst char *c, _Xconst char *d, String *p, Cardinal *np)
{ (void) n; (void) t; (void) c; (void) d; (void) p; (void) np; fuzz_warnings++; }
void XtErrorMsg(_Xconst char *n, _Xconst char *t, _Xconst char *c, _Xconst char *d, String *p, Cardinal *np)
{ (void) n; (void) t; (void) c; (void) d; (void) p; (void) np; fprintf(stderr, "XtErrorMsg\n"); abort(); }
void XtDisplayStringConversionWarning(Display *d, _Xconst char *s, _Xconst char *t)
{ (void) d; (void) s; (void) t; fuzz_warnings++; }
XtAppContext XtDisplayToApplicationContext(Display *d) { (void) d; return NULL; }
XtAppContext XtWidgetToApplicationContext(Widget w) { (void) w; return NULL; }
Display *XtDisplayOfObject(Widget w) { (void) w; return fuzz_display(); }
Screen *XtScreenOfObject(Widget w) { (void) w; fuzz_display(); return &screen_block; }
XrmDatabase XtScreenDatabase(Screen *s) { (void) s; return NULL; }
void XtSetValues(Widget w, ArgList a, Cardinal n) { (void) w; (void) a; (void) n; }
Widget XtNameToWidget(Widget w, _Xconst char *n) { (void) w; (void) n; return NULL; }
Widget XtWindowToWidget(Display *d, Window w) { (void) d; (void) w; return NULL; }
XtCallbackStatus XtHasCallbacks(Widget w, _Xconst char *n) { (void) w; (void) n; return XtCallbackNoList; }
void XtCallCallbackList(Widget w, XtCallbackList l, XtPointer c) { (void) w; (void) l; (void) c; }
Boolean XtIsSubclass(Widget w, WidgetClass c) { (void) w; (void) c; return False; }
Boolean _XtIsSubclassOf(Widget w, WidgetClass c, WidgetClass f, _XtXtEnum flag)
{ (void) w; (void) c; (void) f; (void) flag; return False; }
Boolean XtCallConverter(Display *d, XtTypeConverter c, XrmValuePtr a, Cardinal n, XrmValuePtr f, XrmValuePtr t,
                        XtCacheRef *r)
{ (void) d; (void) c; (void) a; (void) n; (void) f; (void) t; (void) r; return False; }
Boolean XtConvertAndStore(Widget w, _Xconst char *from_type, XrmValuePtr from, _Xconst char *to_type, XrmValuePtr to)
{ (void) w; (void) from_type; (void) from; (void) to_type; (void) to; return False; }
Boolean XtCvtStringToFontSet(Display *d, XrmValuePtr a, Cardinal *n, XrmValuePtr f, XrmValuePtr t, XtPointer *c)
{ (void) d; (void) a; (void) n; (void) f; (void) t; (void) c; return False; }
Boolean XtCvtStringToFontStruct(Display *d, XrmValuePtr a, Cardinal *n, XrmValuePtr f, XrmValuePtr t, XtPointer *c)
{ (void) d; (void) a; (void) n; (void) f; (void) t; (void) c; return False; }
Boolean XtCvtStringToPixel(Display *d, XrmValuePtr a, Cardinal *n, XrmValuePtr f, XrmValuePtr t, XtPointer *c)
{ (void) d; (void) a; (void) n; (void) f; (void) t; (void) c; return False; }
WidgetClass applicationShellWidgetClass, topLevelShellWidgetClass;
WidgetClass xmExtObjectClass;

/* ---- the widget library ---- */

Display *_XmGetDefaultDisplay(void) { return fuzz_display(); }
Widget XmGetXmDisplay(Display *d) { (void) d; return NULL; }
XtPointer XmeTraitGet(XtPointer o, XrmQuark q) { (void) o; (void) q; return NULL; }
XrmQuark XmQTspecifyLayoutDirection, XmQTspecifyUnitType, XmQTspecifyRenderTable, XmQmotif;
#if NeedWidePrototypes
Boolean XmRepTypeValidValue(int id, unsigned int v, Widget w)
#else
Boolean XmRepTypeValidValue(XmRepTypeId id, unsigned char v, Widget w)
#endif
{ (void) id; (void) v; (void) w; return True; }
XmGenericClassExt *_XmGetClassExtensionPtr(XmGenericClassExt *p, XrmQuark q) { (void) p; (void) q; return NULL; }
int _XmGetFontUnit(Display *d, int dim) { (void) d; return dim ? dim : 10; }
void XmeWarning(Widget w, char *m) { (void) w; (void) m; fuzz_warnings++; }
const char *XME_WARNING = "warning";
void _XmAppearanceNoteFontDefault(Widget w, Widget p, unsigned char k) { (void) w; (void) p; (void) k; }
Boolean _XmRepTypeInstallConverters(void) { return True; }
XmFontList XmFontListCreate(XFontStruct *f, XmStringCharSet c) { (void) f; (void) c; return (XmFontList) calloc(1, 16); }
XmFontList XmFontListAdd(XmFontList old, XFontStruct *f, XmStringCharSet c)
{ (void) f; (void) c; return old ? old : (XmFontList) calloc(1, 16); }
XmFontListEntry XmFontListEntryCreate(char *t, XmFontType ty, XtPointer f)
{ (void) t; (void) ty; (void) f; return (XmFontListEntry) calloc(1, 16); }
XmFontListEntry XmFontListEntryLoad(Display *d, char *name, XmFontType type, char *tag)
{ (void) d; (void) name; (void) type; (void) tag; return NULL; }
void XmFontListEntryFree(XmFontListEntry *e) { if (e) { free(*e); *e = NULL; } }
XmFontList XmFontListAppendEntry(XmFontList old, XmFontListEntry e)
{ (void) e; return old ? old : (XmFontList) calloc(1, 16); }
void XmFontListFree(XmFontList l) { free(l); }
