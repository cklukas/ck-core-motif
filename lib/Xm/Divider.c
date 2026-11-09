/* Additive text divider; XmSeparator remains unchanged. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "PaintI.h"

#include <X11/IntrinsicP.h>
#include <limits.h>
#include <string.h>
#include <Xm/Appearance.h>
#include <Xm/DividerP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "XmI.h"

static Position bounded_position(long long value)
{
    if (value < SHRT_MIN) return (Position)SHRT_MIN;
    if (value > SHRT_MAX) return (Position)SHRT_MAX;
    return (Position)value;
}
static unsigned int bounded_extent(int value)
{
    return value > 0 ? (unsigned int)value : 0U;
}

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void BufferedRedisplay(Widget, XEvent *, Region);
static Boolean BufferedSetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static void natural_size(XmDividerWidget, Dimension *, Dimension *);

#define Offset(field) XtOffsetOf(XmDividerRec, field)
static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmDividerWidget divider = (XmDividerWidget)widget;
    (void)offset;
    if (divider->divider.check_set_render_table)
        value->addr = NULL;
    else {
        divider->divider.check_set_render_table = True;
        value->addr = (char *)&divider->divider.font;
    }
}
static XtResource resources[] = {
    {XmNdividerLabel, XmCDividerLabel, XmRXmString, sizeof(XmString),
     Offset(divider.label), XmRImmediate, (XtPointer)NULL},
    {XmNdividerOrientation, XmCDividerOrientation, XmROrientation,
     sizeof(unsigned char), Offset(divider.orientation), XmRImmediate,
     (XtPointer)XmHORIZONTAL},
    {XmNdividerThickness, XmCDividerThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(divider.thickness), XmRImmediate, (XtPointer)1},
    {XmNdividerSpacing, XmCDividerSpacing, XmRHorizontalDimension,
     sizeof(Dimension), Offset(divider.spacing), XmRImmediate, (XtPointer)8},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
     Offset(divider.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmFontList),
     Offset(divider.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)False}
};

externaldef(xmdividerclassrec) XmDividerClassRec xmDividerClassRec = {
    {(WidgetClass)&xmPrimitiveClassRec, "XmDivider", sizeof(XmDividerRec),
     NULL, NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0,
     resources, XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal,
     True, False, Destroy, NULL, BufferedRedisplay, BufferedSetValues, NULL,
     XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
     XtInheritTranslations, QueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
     XtInheritTranslations, NULL, NULL, 0, NULL},
    {NULL}};

externaldef(xmdividerwidgetclass) WidgetClass xmDividerWidgetClass =
    (WidgetClass)&xmDividerClassRec;

static XmString copy_label(XmString source) {
    return source ? XmStringCopy(source) : XmStringCreateLocalized("");
}
static void label_extent(XmDividerWidget divider, Dimension *width, Dimension *height) {
    *width = *height = 0;
    if (divider->divider.label && divider->divider.font)
        XmStringExtent(divider->divider.font, divider->divider.label, width, height);
}
static void natural_size(XmDividerWidget divider, Dimension *width, Dimension *height) {
    Dimension text_width, text_height;
    unsigned long w, h;
    unsigned long padding = _XmDpiDefaultDimension((Widget)divider, 6);
    label_extent(divider, &text_width, &text_height);
    if (divider->divider.orientation == XmHORIZONTAL) {
        w = text_width ? (unsigned long)text_width + 2UL * divider->divider.spacing +
            2UL * _XmDpiDefaultDimension((Widget)divider, 16) :
            _XmDpiDefaultDimension((Widget)divider, 120);
        h = MAX(text_height, divider->divider.thickness) + 2UL * padding;
    } else {
        w = (unsigned long)divider->divider.thickness + 2UL * padding;
        h = _XmDpiDefaultDimension((Widget)divider, 40);
    }
    *width = (Dimension)MIN(w, USHRT_MAX);
    *height = (Dimension)MIN(h, USHRT_MAX);
}
static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmDividerWidget divider = (XmDividerWidget)widget;
    XGCValues values;
    (void)args; (void)count;
    divider->divider.label = copy_label(divider->divider.label);
    if (!divider->divider.font)
        divider->divider.font = XmFontListCopy(
            XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
    else
        divider->divider.font = XmFontListCopy(divider->divider.font);
    if (divider->divider.thickness == 0) divider->divider.thickness = 1;
    values.foreground = divider->primitive.foreground;
    values.background = divider->core.background_pixel;
    divider->divider.foreground_gc = XCreateGC(
        XtDisplay(widget), DefaultRootWindow(XtDisplay(widget)),
        GCForeground | GCBackground, &values);
    if (divider->divider.orientation != XmHORIZONTAL &&
        divider->divider.orientation != XmVERTICAL)
        divider->divider.orientation = XmHORIZONTAL;
    divider->divider.automatic_width = request->core.width == 0;
    divider->divider.automatic_height = request->core.height == 0;
    Dimension width, height;
    natural_size(divider, &width, &height);
    if (divider->divider.automatic_width) divider->core.width = width;
    if (divider->divider.automatic_height) divider->core.height = height;
}
static void Destroy(Widget widget) {
    XmDividerWidget divider = (XmDividerWidget)widget;
    if (divider->divider.foreground_gc)
        XFreeGC(XtDisplay(widget), divider->divider.foreground_gc);
    if (divider->divider.font)
        XmRenderTableFree(divider->divider.font);
    if (divider->divider.label)
        XmStringFree(divider->divider.label);
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmDividerWidget divider = (XmDividerWidget)widget;
    Display *display = XtDisplay(widget);
    Drawable window = _XmPaintDrawable(widget);
    GC gc = divider->divider.foreground_gc;
    int width = divider->core.width, height = divider->core.height;
    Dimension text_width, text_height;
    XmAppearanceProfile profile = XmAPPEARANCE_CLASSIC;
    Pixel foreground = XtIsSensitive(widget) ? divider->primitive.foreground :
                                             divider->primitive.bottom_shadow_color;
    Pixel line;
    (void)event; (void)region;
    if (!window || !divider->divider.foreground_gc) return;
    (void)XmGetAppearanceProfile(display, &profile);
    line = profile == XmAPPEARANCE_CLASSIC ? foreground : divider->primitive.bottom_shadow_color;
    XSetClipMask(display, gc, None);
    XSetForeground(display, gc, line);
    XSetBackground(display, gc, divider->core.background_pixel);

    label_extent(divider, &text_width, &text_height);
    if (divider->divider.orientation == XmHORIZONTAL) {
        int thickness = MIN(divider->divider.thickness, height);
        int y = (height - thickness) / 2;
        if (text_width && text_height) {
            int x = (width - (int)text_width) / 2;
            int left_end = MAX(0, MIN(width, x - divider->divider.spacing));
            int right_start = MAX(0, MIN(width, x + text_width + divider->divider.spacing));
            XRectangle clip = {0, 0, (unsigned short)width, (unsigned short)height};
            if (left_end > 0)
                XFillRectangle(display, window, gc, 0, y,
                               bounded_extent(left_end), bounded_extent(thickness));
            if (right_start < width)
                XFillRectangle(display, window, gc, right_start, y,
                               bounded_extent(width - right_start), bounded_extent(thickness));
            XSetForeground(display, gc, foreground);
                XmStringDraw(display, window, divider->divider.font,
                         divider->divider.label, gc, bounded_position((long long)x),
                         bounded_position(((long long)height - (long long)text_height) / 2),
                         text_width, XmALIGNMENT_BEGINNING,
                         XmPrim_layout_direction(divider), &clip);
        } else {
            XFillRectangle(display, window, gc, 0, y,
                           bounded_extent(width), bounded_extent(thickness));
        }
    } else {
        int thickness = MIN(divider->divider.thickness, width);
        XFillRectangle(display, window, gc, (width - thickness) / 2, 0,
                       bounded_extent(thickness), bounded_extent(height));
    }
    XSetClipMask(display, gc, None);
}

static void BufferedRedisplay(Widget widget, XEvent *event, Region region)
{
    _XmBufferedPaint(widget, Redisplay, event, region);
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmDividerWidget old = (XmDividerWidget)old_widget;
    XmDividerWidget divider = (XmDividerWidget)widget;
    Boolean font_changed = old->divider.font != divider->divider.font;
    Boolean colors_changed = old->primitive.foreground != divider->primitive.foreground ||
                             old->core.background_pixel != divider->core.background_pixel;
    XmString replacement;
    (void)request;
    if (old->divider.label != divider->divider.label) {
        replacement = copy_label(divider->divider.label);
        if (replacement) {
            XmStringFree(old->divider.label);
            divider->divider.label = replacement;
        } else {
            divider->divider.label = old->divider.label;
        }
    }
    if (font_changed) {
        XmFontList font_replacement = divider->divider.font
            ? XmFontListCopy(divider->divider.font)
            : XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (font_replacement) {
            if (old->divider.font) XmRenderTableFree(old->divider.font);
            divider->divider.font = font_replacement;
        } else {
            divider->divider.font = old->divider.font;
            font_changed = False;
        }
    }
    if (!divider->divider.thickness) divider->divider.thickness = 1;
    if (divider->divider.orientation != XmHORIZONTAL &&
        divider->divider.orientation != XmVERTICAL)
        divider->divider.orientation = old->divider.orientation;
    for (Cardinal i = 0; i < *count; ++i) {
        if (!strcmp(args[i].name, XmNwidth))
            divider->divider.automatic_width = (Dimension)args[i].value == 0;
        if (!strcmp(args[i].name, XmNheight))
            divider->divider.automatic_height = (Dimension)args[i].value == 0;
    }
    if (colors_changed && divider->divider.foreground_gc) {
        XSetForeground(XtDisplay(widget), divider->divider.foreground_gc,
                       divider->primitive.foreground);
        XSetBackground(XtDisplay(widget), divider->divider.foreground_gc,
                       divider->core.background_pixel);
    }
    Dimension width, height;
    natural_size(divider, &width, &height);
    if (divider->divider.automatic_width) divider->core.width = width;
    if (divider->divider.automatic_height) divider->core.height = height;
    return old->divider.label != divider->divider.label ||
           old->divider.orientation != divider->divider.orientation ||
           old->divider.thickness != divider->divider.thickness ||
           old->divider.spacing != divider->divider.spacing || font_changed ||
           old->primitive.foreground != divider->primitive.foreground ||
           old->primitive.bottom_shadow_color != divider->primitive.bottom_shadow_color ||
           old->core.background_pixel != divider->core.background_pixel ||
           XtIsSensitive(old_widget) != XtIsSensitive(widget) ||
           XmPrim_layout_direction(old) != XmPrim_layout_direction(divider);
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                       XtWidgetGeometry *reply) {
    if (widget == NULL || reply == NULL || !XmIsDivider(widget))
        return XtGeometryNo;
    XmDividerWidget divider = (XmDividerWidget)widget;
    reply->request_mode = CWWidth | CWHeight;
    natural_size(divider, &reply->width, &reply->height);
    if (!divider->divider.automatic_width) reply->width = divider->core.width;
    if (!divider->divider.automatic_height) reply->height = divider->core.height;
    return XmeReplyToQueryGeometry(widget, request, reply);
}

Widget XmCreateDivider(Widget parent, char *name, ArgList args, Cardinal count) {
    return XtCreateWidget(name, xmDividerWidgetClass, parent, args, count);
}
Widget XmVaCreateDivider(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmDividerWidgetClass,
                                                     parent, False, var, count);
    va_end(var); return widget;
}
Widget XmVaCreateManagedDivider(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmDividerWidgetClass,
                                                     parent, True, var, count);
    va_end(var); return widget;
}

/* Xt's default redisplay request clears the front window. Queue our complete
 * frame instead, after resource and geometry negotiation has finished. */
static Boolean BufferedSetValues(Widget old, Widget request, Widget widget,
                                ArgList args, Cardinal *count)
{
    if (SetValues(old, request, widget, args, count))
        _XmPaintInvalidate(widget);
    return False;
}
