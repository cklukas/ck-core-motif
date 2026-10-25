/* Additive text divider; XmSeparator remains unchanged. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <Xm/DividerP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <string.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);

#define Offset(field) XtOffsetOf(XmDividerRec, field)
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
    char *text;
    XmString result;
    if (!source) return XmStringCreateLocalized("");
    text = XmStringUnparse(source, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
                           NULL, 0, XmOUTPUT_ALL);
    if (!text) return XmStringCreateLocalized("");
    result = XmStringCreateLocalized(text);
    XtFree(text);
    return result;
}
static Dimension label_width(XmDividerWidget divider) {
    char *text = XmStringUnparse(divider->divider.label, NULL,
                                 XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0,
                                 XmOUTPUT_ALL);
    Dimension width = text ? (Dimension)(strlen(text) * 8) : 0;
    XtFree(text);
    return width;
}
static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmDividerWidget divider = (XmDividerWidget)widget;
    XGCValues values;
    (void)request; (void)args; (void)count;
    divider->divider.label = copy_label(divider->divider.label);
    if (divider->divider.thickness == 0) divider->divider.thickness = 1;
    values.foreground = divider->primitive.foreground;
    values.background = divider->core.background_pixel;
    divider->divider.foreground_gc = XCreateGC(
        XtDisplay(widget), DefaultRootWindow(XtDisplay(widget)),
        GCForeground | GCBackground, &values);
    if (!divider->core.width || !divider->core.height) {
        if (divider->divider.orientation == XmHORIZONTAL) {
            divider->core.width = 120;
            divider->core.height = divider->divider.thickness + 12;
        } else {
            divider->core.width = divider->divider.thickness + 12;
            divider->core.height = 40;
        }
    }
}
static void Destroy(Widget widget) {
    XmDividerWidget divider = (XmDividerWidget)widget;
    if (divider->divider.foreground_gc)
        XFreeGC(XtDisplay(widget), divider->divider.foreground_gc);
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmDividerWidget divider = (XmDividerWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    char *text;
    int y, x;
    Dimension label = label_width(divider);
    (void)event; (void)region;
    if (!window) return;
    XClearWindow(display, window);
    text = XmStringUnparse(divider->divider.label, NULL, XmCHARSET_TEXT,
                           XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
    if (divider->divider.orientation == XmHORIZONTAL) {
        y = (int)divider->core.height / 2;
        if (text && *text) {
            x = ((int)divider->core.width - (int)label) / 2;
            XDrawLine(display, window, divider->divider.foreground_gc, 0, y,
                      x - (int)divider->divider.spacing, y);
            XDrawLine(display, window, divider->divider.foreground_gc,
                      x + (int)label + (int)divider->divider.spacing, y,
                      (int)divider->core.width, y);
            XDrawString(display, window, divider->divider.foreground_gc, x,
                        y + 5, text, (int)strlen(text));
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
    XtFree(text);
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmDividerWidget old = (XmDividerWidget)old_widget;
    XmDividerWidget divider = (XmDividerWidget)widget;
    (void)request; (void)args; (void)count;
    if (old->divider.label != divider->divider.label)
        divider->divider.label = copy_label(divider->divider.label);
    if (!divider->divider.thickness) divider->divider.thickness = 1;
    return old->divider.label != divider->divider.label ||
           old->divider.orientation != divider->divider.orientation ||
           old->divider.thickness != divider->divider.thickness ||
           old->divider.spacing != divider->divider.spacing;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                       XtWidgetGeometry *reply) {
    XmDividerWidget divider = (XmDividerWidget)widget;
    Dimension label = label_width(divider);
    (void)request;
    reply->request_mode = CWWidth | CWHeight;
    if (divider->divider.orientation == XmHORIZONTAL) {
        reply->width = label ? label + 2 * divider->divider.spacing + 16 : 120;
        reply->height = divider->divider.thickness + 12;
    } else {
        reply->width = divider->divider.thickness + 12;
        reply->height = 40;
    }
    return (widget->core.width == reply->width && widget->core.height == reply->height)
               ? XtGeometryYes : XtGeometryAlmost;
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
