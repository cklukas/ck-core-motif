#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <X11/Xlib.h>
#include <Xm/DropHighlightP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "XmI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);

#define Offset(field) XtOffsetOf(XmDropHighlightRec, drop_highlight.field)
static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmDropHighlightWidget highlight = (XmDropHighlightWidget)widget;
    (void)offset;
    if (highlight->drop_highlight.check_set_render_table)
        value->addr = NULL;
    else {
        highlight->drop_highlight.check_set_render_table = True;
        value->addr = (char *)&highlight->drop_highlight.font;
    }
}
static XtResource resources[] = {
    {XmNdropHighlightState, XmCDropHighlightState, XmRUnsignedChar,
     sizeof(unsigned char), Offset(state), XmRImmediate,
     (XtPointer)XmDROP_HIGHLIGHT_NONE},
    {XmNdropHighlightThickness, XmCDropHighlightThickness,
     XmRHorizontalDimension, sizeof(Dimension), Offset(thickness),
     XmRImmediate, (XtPointer)2},
    {XmNdropHighlightLabel, XmCDropHighlightLabel, XmRXmString,
     sizeof(XmString), Offset(label), XmRImmediate, NULL},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
     Offset(font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmFontList),
     Offset(font), XmRCallProc, (XtPointer)CheckSetRenderTable}
};

externaldef(xmdrophighlightclassrec) XmDropHighlightClassRec
xmDropHighlightClassRec = {
    {(WidgetClass)&xmPrimitiveClassRec, "XmDropHighlight",
     sizeof(XmDropHighlightRec), NULL, NULL, False, Initialize, NULL,
     XtInheritRealize, NULL, 0, resources, XtNumber(resources), NULLQUARK,
     True, XtExposeCompressMaximal, True, False, Destroy, NULL, Redisplay,
     SetValues, NULL, XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
     XtInheritTranslations, QueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
     XtInheritTranslations, NULL, NULL, 0, NULL},
    {NULL}
};

externaldef(xmdrophighlightwidgetclass) WidgetClass xmDropHighlightWidgetClass =
    (WidgetClass)&xmDropHighlightClassRec;

static Boolean valid_state(unsigned char state)
{
    return state == XmDROP_HIGHLIGHT_NONE ||
           state == XmDROP_HIGHLIGHT_VALID ||
           state == XmDROP_HIGHLIGHT_INVALID;
}

static XmString copy_string(XmString value)
{
    if (!value)
        return NULL;
    return XmStringCopy(value);
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count)
{
    XmDropHighlightWidget highlight = (XmDropHighlightWidget)widget;
    (void)request;
    (void)args;
    (void)count;
    if (!valid_state(highlight->drop_highlight.state))
        highlight->drop_highlight.state = XmDROP_HIGHLIGHT_NONE;
    if (!highlight->core.width)
        highlight->core.width = 1;
    if (!highlight->core.height)
        highlight->core.height = 1;
    highlight->drop_highlight.label = copy_string(
        highlight->drop_highlight.label);
    if (!highlight->drop_highlight.font)
        highlight->drop_highlight.font = XmFontListCopy(
            XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
    else
        highlight->drop_highlight.font = XmFontListCopy(
            highlight->drop_highlight.font);
}

static void Destroy(Widget widget)
{
    XmDropHighlightWidget highlight = (XmDropHighlightWidget)widget;
    XmStringFree(highlight->drop_highlight.label);
    highlight->drop_highlight.label = NULL;
    if (highlight->drop_highlight.font)
        XmRenderTableFree(highlight->drop_highlight.font);
    highlight->drop_highlight.font = NULL;
}

static void Redisplay(Widget widget, XEvent *event, Region region)
{
    XmDropHighlightWidget highlight = (XmDropHighlightWidget)widget;
    XGCValues values;
    GC gc;
    Display *display;
    Window window;
    Dimension thickness;
    Dimension label_width = 0, label_height = 0;
    int inset, label_x, label_y;

    (void)event;
    (void)region;
    if (!XtIsRealized(widget) || highlight->drop_highlight.state ==
        XmDROP_HIGHLIGHT_NONE)
        return;

    display = XtDisplay(widget);
    window = XtWindow(widget);
    thickness = highlight->drop_highlight.thickness;
    if (!thickness)
        return;
    values.foreground = highlight->primitive.highlight_color;
    values.background = highlight->core.background_pixel;
    values.line_width = thickness;
    values.line_style = highlight->drop_highlight.state ==
                        XmDROP_HIGHLIGHT_INVALID ? LineOnOffDash : LineSolid;
    gc = XCreateGC(display, window,
                   GCForeground | GCBackground | GCLineWidth | GCLineStyle,
                   &values);
    if (highlight->drop_highlight.label && highlight->drop_highlight.font)
        XmStringExtent(highlight->drop_highlight.font,
                       highlight->drop_highlight.label,
                       &label_width, &label_height);
    label_x = ((int)highlight->core.width - (int)label_width) / 2;
    label_y = ((int)highlight->core.height - (int)label_height) / 2;
    inset = (int)thickness / 2;
    XDrawRectangle(display, window, gc, inset, inset,
                   highlight->core.width > thickness
                       ? highlight->core.width - thickness : 0,
                   highlight->core.height > thickness
                       ? highlight->core.height - thickness : 0);
    if (label_width && label_height)
        XmStringDraw(display, window, highlight->drop_highlight.font,
                     highlight->drop_highlight.label, gc, label_x, label_y,
                     label_width, XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(highlight), NULL);
    XFreeGC(display, gc);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count)
{
    XmDropHighlightWidget old = (XmDropHighlightWidget)old_widget;
    XmDropHighlightWidget highlight = (XmDropHighlightWidget)widget;
    Boolean redraw = False;

    (void)request;
    (void)args;
    (void)count;
    if (!valid_state(highlight->drop_highlight.state))
        highlight->drop_highlight.state = XmDROP_HIGHLIGHT_NONE;
    if (old->drop_highlight.state != highlight->drop_highlight.state ||
        old->drop_highlight.thickness != highlight->drop_highlight.thickness)
        redraw = True;
    if (old->drop_highlight.label != highlight->drop_highlight.label) {
        XmStringFree(old->drop_highlight.label);
        highlight->drop_highlight.label = copy_string(
            highlight->drop_highlight.label);
        redraw = True;
    }
    if (old->drop_highlight.font != highlight->drop_highlight.font) {
        XmFontList replacement = highlight->drop_highlight.font
            ? XmFontListCopy(highlight->drop_highlight.font)
            : XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (old->drop_highlight.font)
            XmRenderTableFree(old->drop_highlight.font);
        highlight->drop_highlight.font = replacement;
        redraw = True;
    }
    return redraw;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                       XtWidgetGeometry *reply)
{
    (void)widget;
    reply->request_mode = CWWidth | CWHeight;
    XmDropHighlightWidget highlight = (XmDropHighlightWidget)widget;
    Dimension label_width = 0, label_height = 0;
    if (highlight->drop_highlight.label && highlight->drop_highlight.font)
        XmStringExtent(highlight->drop_highlight.font,
                       highlight->drop_highlight.label,
                       &label_width, &label_height);
    reply->width = label_width + 2 * (highlight->drop_highlight.thickness + 4);
    reply->height = label_height + 2 * (highlight->drop_highlight.thickness + 4);
    if (!reply->width) reply->width = 1;
    if (!reply->height) reply->height = 1;
    if (request && (request->request_mode & (CWWidth | CWHeight)) ==
        (CWWidth | CWHeight) && request->width == reply->width &&
        request->height == reply->height)
        return XtGeometryYes;
    return XtGeometryAlmost;
}

Widget XmCreateDropHighlight(Widget parent, char *name, ArgList args,
                             Cardinal count)
{
    return XtCreateWidget(name, xmDropHighlightWidgetClass, parent, args,
                          count);
}

Widget XmVaCreateDropHighlight(Widget parent, char *name, ...)
{
    Widget widget;
    va_list values;
    int count;

    Va_start(values, name);
    count = XmeCountVaListSimple(values);
    va_end(values);
    Va_start(values, name);
    widget = XmeVLCreateWidget(name, xmDropHighlightWidgetClass, parent, False,
                               values, count);
    va_end(values);
    return widget;
}

Widget XmVaCreateManagedDropHighlight(Widget parent, char *name, ...)
{
    Widget widget;
    va_list values;
    int count;

    Va_start(values, name);
    count = XmeCountVaListSimple(values);
    va_end(values);
    Va_start(values, name);
    widget = XmeVLCreateWidget(name, xmDropHighlightWidgetClass, parent, True,
                               values, count);
    va_end(values);
    return widget;
}

Boolean XmDropHighlightSetState(Widget widget, unsigned char state)
{
    if (!widget || !XmIsDropHighlight(widget) || !valid_state(state))
        return False;
    if (((XmDropHighlightWidget)widget)->drop_highlight.state != state) {
        ((XmDropHighlightWidget)widget)->drop_highlight.state = state;
        if (XtIsRealized(widget))
            XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True);
    }
    return True;
}

unsigned char XmDropHighlightGetState(Widget widget)
{
    if (!widget || !XmIsDropHighlight(widget))
        return XmDROP_HIGHLIGHT_NONE;
    return ((XmDropHighlightWidget)widget)->drop_highlight.state;
}
