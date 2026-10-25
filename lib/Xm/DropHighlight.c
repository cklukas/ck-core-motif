#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <X11/Xlib.h>
#include <Xm/DropHighlightP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);

#define Offset(field) XtOffsetOf(XmDropHighlightRec, drop_highlight.field)
static XtResource resources[] = {
    {XmNdropHighlightState, XmCDropHighlightState, XmRUnsignedChar,
     sizeof(unsigned char), Offset(state), XmRImmediate,
     (XtPointer)XmDROP_HIGHLIGHT_NONE},
    {XmNdropHighlightThickness, XmCDropHighlightThickness,
     XmRHorizontalDimension, sizeof(Dimension), Offset(thickness),
     XmRImmediate, (XtPointer)2},
    {XmNdropHighlightLabel, XmCDropHighlightLabel, XmRXmString,
     sizeof(XmString), Offset(label), XmRImmediate, NULL}
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
    char *text;
    XmString result;

    if (!value)
        return NULL;
    text = XmStringUnparse(value, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
                           NULL, 0, XmOUTPUT_ALL);
    result = XmStringCreateLocalized(text ? text : (char *)"");
    XtFree(text);
    return result;
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
}

static void Destroy(Widget widget)
{
    XmDropHighlightWidget highlight = (XmDropHighlightWidget)widget;
    XmStringFree(highlight->drop_highlight.label);
    highlight->drop_highlight.label = NULL;
}

static void Redisplay(Widget widget, XEvent *event, Region region)
{
    XmDropHighlightWidget highlight = (XmDropHighlightWidget)widget;
    XGCValues values;
    GC gc;
    Display *display;
    Window window;
    Dimension thickness;
    int inset;

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
    inset = (int)thickness / 2;
    XDrawRectangle(display, window, gc, inset, inset,
                   highlight->core.width > thickness
                       ? highlight->core.width - thickness : 0,
                   highlight->core.height > thickness
                       ? highlight->core.height - thickness : 0);
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
    }
    return redraw;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                       XtWidgetGeometry *reply)
{
    (void)widget;
    reply->request_mode = CWWidth | CWHeight;
    reply->width = 1;
    reply->height = 1;
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
