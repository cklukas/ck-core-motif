/* Additive icon-and-text presentation label. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <Xm/IconLabelP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <limits.h>
#include "XmI.h"
#include <string.h>

static Position bounded_position(long long value) {
    if (value < SHRT_MIN) return (Position)SHRT_MIN;
    if (value > SHRT_MAX) return (Position)SHRT_MAX;
    return (Position)value;
}

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);

#define Offset(field) XtOffsetOf(XmIconLabelRec, field)
static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmIconLabelWidget label = (XmIconLabelWidget)widget;
    (void)offset;
    if (label->icon_label.check_set_render_table)
        value->addr = NULL;
    else {
        label->icon_label.check_set_render_table = True;
        value->addr = (char *)&label->icon_label.render_table;
    }
}
static XtResource resources[] = {
    {XmNiconLabelLabel, XmCIconLabelLabel, XmRXmString, sizeof(XmString),
     Offset(icon_label.label), XmRImmediate, (XtPointer)NULL},
    {XmNiconLabelPixmap, XmCIconLabelPixmap, XmRDynamicPixmap, sizeof(Pixmap),
     Offset(icon_label.pixmap), XmRImmediate, (XtPointer)XmUNSPECIFIED_PIXMAP},
    {XmNiconLabelSpacing, XmCIconLabelSpacing, XmRHorizontalDimension,
     sizeof(Dimension), Offset(icon_label.spacing), XmRImmediate, (XtPointer)6},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
     Offset(icon_label.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmFontList),
     Offset(icon_label.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
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
    return source ? XmStringCopy(source) : XmStringCreateLocalized("");
}
static Dimension padding(XmIconLabelWidget label) {
    unsigned long frame = (unsigned long)label->primitive.highlight_thickness +
                          label->primitive.shadow_thickness;
    return (Dimension)MIN(USHRT_MAX, MAX(frame,
        (unsigned long)_XmDpiDefaultDimension((Widget)label, 6)));
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
    Dimension tw=0, th=0, inset=padding(label);
    Boolean have_icon = pixmap_size(label, &icon_width, &icon_height);
    if (label->icon_label.render_table && label->icon_label.label)
        XmStringExtent(label->icon_label.render_table, label->icon_label.label, &tw, &th);
    unsigned long total_width = (unsigned long)tw + 2UL*inset;
    if (have_icon)
        total_width += icon_width + (tw ? (unsigned long)label->icon_label.spacing : 0);
    unsigned long total_height = MAX((unsigned long)th,icon_height) + 2UL*inset;
    *width = (Dimension)(total_width > USHRT_MAX ? USHRT_MAX : MAX(1,total_width));
    *height = (Dimension)(total_height > USHRT_MAX ? USHRT_MAX : MAX(1,total_height));
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmIconLabelWidget label = (XmIconLabelWidget)widget;
    XGCValues values;
    (void)args; (void)count;
    label->icon_label.label = copy_label(label->icon_label.label);
    if (!label->icon_label.render_table)
        label->icon_label.render_table = XmFontListCopy(
            XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
    else
        label->icon_label.render_table = XmFontListCopy(label->icon_label.render_table);
    values.foreground = label->primitive.foreground;
    values.background = label->core.background_pixel;
    label->icon_label.foreground_gc = XCreateGC(
        XtDisplay(widget), DefaultRootWindow(XtDisplay(widget)),
        GCForeground | GCBackground, &values);
    label->icon_label.automatic_width = request->core.width == 0;
    label->icon_label.automatic_height = request->core.height == 0;
    {
        Dimension width, height;
        dimensions(label, &width, &height);
        if (label->icon_label.automatic_width) label->core.width = width;
        if (label->icon_label.automatic_height) label->core.height = height;
    }
}

static void Destroy(Widget widget) {
    XmIconLabelWidget label = (XmIconLabelWidget)widget;
    if (label->icon_label.foreground_gc)
        XFreeGC(XtDisplay(widget), label->icon_label.foreground_gc);
    if (label->icon_label.render_table)
        XmRenderTableFree(label->icon_label.render_table);
    if (label->icon_label.label)
        XmStringFree(label->icon_label.label);
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmIconLabelWidget label = (XmIconLabelWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    unsigned int icon_width = 0, icon_height = 0;
    Boolean have_icon = pixmap_size(label, &icon_width, &icon_height);
    Boolean rtl = XmDirectionMatchPartial(XmPrim_layout_direction(label),
                                          XmRIGHT_TO_LEFT, XmHORIZONTAL_MASK);
    XmString value = label->icon_label.label;
    Dimension value_width = 0, value_height = 0, inset = padding(label);
    int left = inset, right = (int)label->core.width - inset;
    int top = inset, bottom = (int)label->core.height - inset;
    (void)event; (void)region;
    if (!window || !label->icon_label.foreground_gc) return;
    XSetForeground(display, label->icon_label.foreground_gc,
        XtIsSensitive(widget) ? label->primitive.foreground : label->primitive.bottom_shadow_color);
    XSetBackground(display, label->icon_label.foreground_gc, label->core.background_pixel);
    XClearWindow(display, window);
    if (right <= left || bottom <= top) return;
    if (value && label->icon_label.render_table)
        XmStringExtent(label->icon_label.render_table, value,
                       &value_width, &value_height);
    if (have_icon) {
        int x = rtl ? right - (int)icon_width : left;
        int y = ((int)label->core.height - (int)icon_height) / 2;
        int x1 = MAX(left, x), y1 = MAX(top, y);
        int x2 = MIN(right, x + (int)icon_width);
        int y2 = MIN(bottom, y + (int)icon_height);
        /* Intersect before the copy: source offsets and destination bounds
         * remain valid even when the icon exceeds the allocated content. */
        if (x2 > x1 && y2 > y1)
            XCopyArea(display, label->icon_label.pixmap, window,
                      label->icon_label.foreground_gc, x1 - x, y1 - y,
                      x2 - x1, y2 - y1, x1, y1);
        if (rtl)
            right = x - (value_width ? label->icon_label.spacing : 0);
        else
            left = x + icon_width + (value_width ? label->icon_label.spacing : 0);
    }
    if (right > left && value && label->icon_label.render_table) {
        XRectangle clip;
        clip.x = (short)left; clip.y = (short)top;
        clip.width = right - left; clip.height = bottom - top;
        XmStringDraw(display, window, label->icon_label.render_table, value,
                     label->icon_label.foreground_gc,
                     bounded_position(left),
                     bounded_position(((long long)label->core.height - (long long)value_height) / 2), clip.width,
                     XmALIGNMENT_BEGINNING, XmPrim_layout_direction(label), &clip);
    }
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmIconLabelWidget old = (XmIconLabelWidget)old_widget;
    XmIconLabelWidget label = (XmIconLabelWidget)widget;
    Boolean render_changed = old->icon_label.render_table != label->icon_label.render_table;
    Boolean label_changed = old->icon_label.label != label->icon_label.label;
    Boolean colors_changed = old->primitive.foreground != label->primitive.foreground ||
                             old->core.background_pixel != label->core.background_pixel;
    (void)request; (void)args; (void)count;
    if (colors_changed && label->icon_label.foreground_gc) {
        XSetForeground(XtDisplay(widget), label->icon_label.foreground_gc,
                       label->primitive.foreground);
        XSetBackground(XtDisplay(widget), label->icon_label.foreground_gc,
                       label->core.background_pixel);
    }
    if (label_changed) {
        XmString replacement = copy_label(label->icon_label.label);
        if (replacement)
            label->icon_label.label = replacement;
        else
            label->icon_label.label = old->icon_label.label;
    }
    if (render_changed) {
        XmFontList replacement = label->icon_label.render_table
            ? XmFontListCopy(label->icon_label.render_table)
            : XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (replacement) {
            if (old->icon_label.render_table)
                XmRenderTableFree(old->icon_label.render_table);
            label->icon_label.render_table = replacement;
        } else {
            label->icon_label.render_table = old->icon_label.render_table;
            render_changed = False;
        }
    }
    for (Cardinal i=0; i<*count; ++i) {
        if (!strcmp(args[i].name, XmNwidth))
            label->icon_label.automatic_width = (Dimension)args[i].value == 0;
        if (!strcmp(args[i].name, XmNheight))
            label->icon_label.automatic_height = (Dimension)args[i].value == 0;
    }
    {
        Dimension width, height;
        dimensions(label, &width, &height);
        if (label->icon_label.automatic_width) label->core.width = width;
        if (label->icon_label.automatic_height) label->core.height = height;
    }
    return (label_changed && label->icon_label.label != old->icon_label.label) ||
           old->icon_label.pixmap != label->icon_label.pixmap ||
           old->icon_label.spacing != label->icon_label.spacing || render_changed ||
           old->primitive.foreground != label->primitive.foreground ||
           old->primitive.bottom_shadow_color != label->primitive.bottom_shadow_color ||
           old->core.background_pixel != label->core.background_pixel ||
           XtIsSensitive(old_widget) != XtIsSensitive(widget);
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                       XtWidgetGeometry *reply) {
    XmIconLabelWidget label = (XmIconLabelWidget)widget;
    reply->request_mode = CWWidth | CWHeight;
    dimensions(label, &reply->width, &reply->height);
    if (!label->icon_label.automatic_width) reply->width = label->core.width;
    if (!label->icon_label.automatic_height) reply->height = label->core.height;
    return XmeReplyToQueryGeometry(widget, request, reply);
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
