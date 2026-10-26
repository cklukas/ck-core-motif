/* Additive text divider; XmSeparator remains unchanged. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <limits.h>
#include <Xm/DividerP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "XmI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static Dimension divider_thickness_span(Dimension thickness);

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
     True, False, Destroy, NULL, Redisplay, SetValues, NULL,
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
static Dimension label_width(XmDividerWidget divider) {
    Dimension width = 0, height = 0;
    if (divider->divider.label && divider->divider.font)
        XmStringExtent(divider->divider.font, divider->divider.label,
                       &width, &height);
    return width;
}
static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmDividerWidget divider = (XmDividerWidget)widget;
    XGCValues values;
    (void)request; (void)args; (void)count;
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
    if (!divider->core.width || !divider->core.height) {
        if (divider->divider.orientation == XmHORIZONTAL) {
            divider->core.width = 120;
            divider->core.height = divider_thickness_span(divider->divider.thickness);
        } else {
            divider->core.width = divider_thickness_span(divider->divider.thickness);
            divider->core.height = 40;
        }
    }
}
static void Destroy(Widget widget) {
    XmDividerWidget divider = (XmDividerWidget)widget;
    if (divider->divider.foreground_gc)
        XFreeGC(XtDisplay(widget), divider->divider.foreground_gc);
    if (divider->divider.font)
        XmRenderTableFree(divider->divider.font);
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmDividerWidget divider = (XmDividerWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    int y, x;
    Dimension label_height = 0;
    Dimension label = label_width(divider);
    (void)event; (void)region;
    if (!window || !divider->divider.foreground_gc) return;
    XClearWindow(display, window);
    if (divider->divider.orientation == XmHORIZONTAL) {
        y = (int)divider->core.height / 2;
        if (label && divider->divider.font) {
            XmStringExtent(divider->divider.font, divider->divider.label,
                           &label, &label_height);
            x = ((int)divider->core.width - (int)label) / 2;
            XDrawLine(display, window, divider->divider.foreground_gc, 0, y,
                      x - (int)divider->divider.spacing, y);
            XDrawLine(display, window, divider->divider.foreground_gc,
                      x + (int)label + (int)divider->divider.spacing, y,
                      (int)divider->core.width, y);
            XmStringDraw(display, window, divider->divider.font,
                         divider->divider.label, divider->divider.foreground_gc,
                         x, ((int)divider->core.height - (int)label_height) / 2,
                         label, XmALIGNMENT_BEGINNING,
                         XmPrim_layout_direction(divider), NULL);
        } else {
            XFillRectangle(display, window, divider->divider.foreground_gc, 0,
                           y - (int)divider->divider.thickness / 2,
                           divider->core.width, divider->divider.thickness);
        }
    } else {
        x = (int)divider->core.width / 2;
        XFillRectangle(display, window, divider->divider.foreground_gc,
                       x - (int)divider->divider.thickness / 2, 0,
                       divider->divider.thickness, divider->core.height);
    }
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmDividerWidget old = (XmDividerWidget)old_widget;
    XmDividerWidget divider = (XmDividerWidget)widget;
    Boolean font_changed = old->divider.font != divider->divider.font;
    XmString replacement;
    (void)request; (void)args; (void)count;
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
        XmFontList replacement = divider->divider.font
            ? XmFontListCopy(divider->divider.font)
            : XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (old->divider.font) XmRenderTableFree(old->divider.font);
        divider->divider.font = replacement;
    }
    if (!divider->divider.thickness) divider->divider.thickness = 1;
    return old->divider.label != divider->divider.label ||
           old->divider.orientation != divider->divider.orientation ||
           old->divider.thickness != divider->divider.thickness ||
           old->divider.spacing != divider->divider.spacing || font_changed;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                       XtWidgetGeometry *reply) {
    XmDividerWidget divider = (XmDividerWidget)widget;
    Dimension label = label_width(divider);
    (void)request;
    reply->request_mode = CWWidth | CWHeight;
    if (divider->divider.orientation == XmHORIZONTAL) {
        unsigned long requested = label ? (unsigned long)label +
            2UL * (unsigned long)divider->divider.spacing + 16UL : 120UL;
        reply->width = (Dimension)(requested > USHRT_MAX ? USHRT_MAX : requested);
        reply->height = divider_thickness_span(divider->divider.thickness);
    } else {
        reply->width = divider_thickness_span(divider->divider.thickness);
        reply->height = 40;
    }
    return (widget->core.width == reply->width && widget->core.height == reply->height)
               ? XtGeometryYes : XtGeometryAlmost;
}

static Dimension
divider_thickness_span(Dimension thickness)
{
    return thickness > (Dimension)(USHRT_MAX - 12) ?
        (Dimension)USHRT_MAX : (Dimension)(thickness + 12);
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
