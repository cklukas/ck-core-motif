/* Additive icon-and-text presentation label. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <Xm/IconLabelP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <limits.h>
#include <string.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);

#define Offset(field) XtOffsetOf(XmIconLabelRec, field)
static XtResource resources[] = {
    {XmNiconLabelLabel, XmCIconLabelLabel, XmRXmString, sizeof(XmString),
     Offset(icon_label.label), XmRImmediate, (XtPointer)NULL},
    {XmNiconLabelPixmap, XmCIconLabelPixmap, XmRDynamicPixmap, sizeof(Pixmap),
     Offset(icon_label.pixmap), XmRImmediate, (XtPointer)XmUNSPECIFIED_PIXMAP},
    {XmNiconLabelSpacing, XmCIconLabelSpacing, XmRHorizontalDimension,
     sizeof(Dimension), Offset(icon_label.spacing), XmRImmediate, (XtPointer)6},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)False}
};

externaldef(xmiconlabelclassrec) XmIconLabelClassRec xmIconLabelClassRec = {
    {(WidgetClass)&xmPrimitiveClassRec, "XmIconLabel", sizeof(XmIconLabelRec),
     NULL, NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0,
     resources, XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal,
     True, False, Destroy, NULL, Redisplay, SetValues, NULL,
     XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
     XtInheritTranslations, QueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
     XtInheritTranslations, NULL, NULL, 0, NULL},
    {NULL}};

externaldef(xmiconlabelwidgetclass) WidgetClass xmIconLabelWidgetClass =
    (WidgetClass)&xmIconLabelClassRec;

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

static Dimension text_width(XmIconLabelWidget label) {
    char *text = XmStringUnparse(label->icon_label.label, NULL,
                                 XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0,
                                 XmOUTPUT_ALL);
    size_t length = text ? strlen(text) : 0;
    Dimension width = length > (USHRT_MAX / 8) ? USHRT_MAX :
                      (Dimension)(length * 8);
    XtFree(text);
    return width;
}

static Boolean pixmap_size(XmIconLabelWidget label, unsigned int *width,
                           unsigned int *height) {
    Window root;
    int x, y;
    unsigned int border, depth;
    if (label->icon_label.pixmap == XmUNSPECIFIED_PIXMAP ||
        label->icon_label.pixmap == None)
        return False;
    return XGetGeometry(XtDisplay((Widget)label), label->icon_label.pixmap,
                        &root, &x, &y, width, height, &border, &depth);
}

static void dimensions(XmIconLabelWidget label, Dimension *width,
                       Dimension *height) {
    unsigned int icon_width = 0, icon_height = 0;
    Dimension text = text_width(label);
    Boolean have_icon = pixmap_size(label, &icon_width, &icon_height);
    unsigned long total_width = (unsigned long)text + 12;
    if (have_icon)
        total_width += (unsigned long)icon_width +
                       label->icon_label.spacing;
    *width = (Dimension)(total_width > USHRT_MAX ? USHRT_MAX : total_width);
    *height = (Dimension)(icon_height > 16 ? icon_height : 16) + 12;
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmIconLabelWidget label = (XmIconLabelWidget)widget;
    XGCValues values;
    (void)request; (void)args; (void)count;
    label->icon_label.label = copy_label(label->icon_label.label);
    values.foreground = label->primitive.foreground;
    values.background = label->core.background_pixel;
    label->icon_label.foreground_gc = XCreateGC(
        XtDisplay(widget), DefaultRootWindow(XtDisplay(widget)),
        GCForeground | GCBackground, &values);
    if (!label->core.width || !label->core.height)
        dimensions(label, &label->core.width, &label->core.height);
}

static void Destroy(Widget widget) {
    XmIconLabelWidget label = (XmIconLabelWidget)widget;
    if (label->icon_label.foreground_gc)
        XFreeGC(XtDisplay(widget), label->icon_label.foreground_gc);
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmIconLabelWidget label = (XmIconLabelWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    unsigned int icon_width = 0, icon_height = 0;
    Dimension text = text_width(label);
    Boolean have_icon = pixmap_size(label, &icon_width, &icon_height);
    XmDirection direction = XmLEFT_TO_RIGHT;
    char *value;
    int x, y;
    (void)event; (void)region;
    if (!window) return;
    XtVaGetValues(widget, XmNlayoutDirection, &direction, NULL);
    XClearWindow(display, window);
    value = XmStringUnparse(label->icon_label.label, NULL, XmCHARSET_TEXT,
                            XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
    y = ((int)label->core.height - 16) / 2 + 12;
    if (XmDirectionMatchPartial(direction, XmRIGHT_TO_LEFT,
                                XmHORIZONTAL_MASK)) {
        x = (int)label->core.width - 6;
        if (value) {
            x -= (int)text;
            XDrawString(display, window, label->icon_label.foreground_gc,
                        x, y, value, (int)strlen(value));
        }
        if (have_icon)
            XCopyArea(display, label->icon_label.pixmap, window,
                      label->icon_label.foreground_gc, 0, 0, icon_width,
                      icon_height, x - (int)label->icon_label.spacing -
                      (int)icon_width, ((int)label->core.height -
                      (int)icon_height) / 2);
    } else {
        x = 6;
        if (have_icon) {
            XCopyArea(display, label->icon_label.pixmap, window,
                      label->icon_label.foreground_gc, 0, 0, icon_width,
                      icon_height, x, ((int)label->core.height -
                      (int)icon_height) / 2);
            x += (int)icon_width + (int)label->icon_label.spacing;
        }
        if (value)
            XDrawString(display, window, label->icon_label.foreground_gc,
                        x, y, value, (int)strlen(value));
    }
    XtFree(value);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmIconLabelWidget old = (XmIconLabelWidget)old_widget;
    XmIconLabelWidget label = (XmIconLabelWidget)widget;
    (void)request; (void)args; (void)count;
    if (old->icon_label.label != label->icon_label.label)
        label->icon_label.label = copy_label(label->icon_label.label);
    return old->icon_label.label != label->icon_label.label ||
           old->icon_label.pixmap != label->icon_label.pixmap ||
           old->icon_label.spacing != label->icon_label.spacing;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                       XtWidgetGeometry *reply) {
    XmIconLabelWidget label = (XmIconLabelWidget)widget;
    Dimension width, height;
    (void)request;
    dimensions(label, &width, &height);
    reply->request_mode = CWWidth | CWHeight;
    reply->width = width; reply->height = height;
    return (widget->core.width == width && widget->core.height == height)
               ? XtGeometryYes : XtGeometryAlmost;
}

Widget XmCreateIconLabel(Widget parent, char *name, ArgList args,
                         Cardinal count) {
    return XtCreateWidget(name, xmIconLabelWidgetClass, parent, args, count);
}
Widget XmVaCreateIconLabel(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmIconLabelWidgetClass,
                                                     parent, False, var, count);
    va_end(var); return widget;
}
Widget XmVaCreateManagedIconLabel(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmIconLabelWidgetClass,
                                                     parent, True, var, count);
    va_end(var); return widget;
}
