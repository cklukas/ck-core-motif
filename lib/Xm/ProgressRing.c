/* Additive compact progress ring.  It shares the ProgressBar value contract. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <limits.h>
#include <X11/IntrinsicP.h>
#include <Xm/ProgressRingP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "XmI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void ScaleChanged(Widget, double, double, XtPointer);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);

#define Offset(field) XtOffsetOf(XmProgressRingRec, field)
static XtResource resources[] = {
    {XmNprogressMinimum, XmCProgressMinimum, XmRInt, sizeof(int),
     Offset(progress_ring.minimum), XmRImmediate, (XtPointer)0},
    {XmNprogressMaximum, XmCProgressMaximum, XmRInt, sizeof(int),
     Offset(progress_ring.maximum), XmRImmediate, (XtPointer)100},
    {XmNprogressValue, XmCProgressValue, XmRInt, sizeof(int),
     Offset(progress_ring.value), XmRImmediate, (XtPointer)0},
    {XmNprogressIndeterminate, XmCProgressIndeterminate, XmRBoolean,
     sizeof(Boolean), Offset(progress_ring.indeterminate), XmRImmediate,
     (XtPointer)False},
    {XmNprogressLabelString, XmCProgressLabelString, XmRXmString,
     sizeof(XmString), Offset(progress_ring.label), XmRImmediate,
     (XtPointer)NULL},
    {XmNprogressTextVisible, XmCProgressTextVisible, XmRBoolean,
     sizeof(Boolean), Offset(progress_ring.text_visible), XmRImmediate,
     (XtPointer)True},
    {XmNprogressValueChangedCallback, XmCProgressValueChangedCallback,
     XmRCallback, sizeof(XtCallbackList),
     Offset(progress_ring.value_changed_callback), XmRPointer,
     (XtPointer)NULL},
    {XmNspacing, XmCSpacing, XmRHorizontalDimension, sizeof(Dimension),
     Offset(progress_ring.spacing), XmRImmediate, (XtPointer)8},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};

externaldef(xmprogressringclassrec) XmProgressRingClassRec
    xmProgressRingClassRec = {
        {
            (WidgetClass)&xmPrimitiveClassRec, "XmProgressRing",
            sizeof(XmProgressRingRec), NULL, NULL, False, Initialize, NULL,
            XtInheritRealize, NULL, 0, resources, XtNumber(resources),
            NULLQUARK, True, XtExposeCompressMaximal, True, False, Destroy,
            NULL, Redisplay, SetValues, NULL, XtInheritSetValuesAlmost, NULL,
            NULL, XtVersion, NULL, XtInheritTranslations, QueryGeometry,
            NULL, NULL
        },
        {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
         XtInheritTranslations, NULL, NULL, 0, NULL},
        {NULL}
    };
externaldef(xmprogressringwidgetclass) WidgetClass xmProgressRingWidgetClass =
    (WidgetClass)&xmProgressRingClassRec;

static void Normalize(XmProgressRingWidget ring) {
    if (ring->progress_ring.maximum < ring->progress_ring.minimum)
        ring->progress_ring.maximum = ring->progress_ring.minimum;
    if (ring->progress_ring.value < ring->progress_ring.minimum)
        ring->progress_ring.value = ring->progress_ring.minimum;
    if (ring->progress_ring.value > ring->progress_ring.maximum)
        ring->progress_ring.value = ring->progress_ring.maximum;
}

static Dimension bounded_dimension(unsigned long value) {
    return (Dimension)(value > USHRT_MAX ? USHRT_MAX : value);
}

static void UpdateGCs(XmProgressRingWidget ring) {
    Display *display = XtDisplay((Widget)ring);
    Dimension thickness = _XmDpiDefaultDimension((Widget)ring, 4);
    if (thickness == 0) thickness = 1;
    if (ring->progress_ring.track_gc) {
        XSetForeground(display, ring->progress_ring.track_gc,
                       ring->primitive.bottom_shadow_color);
        XSetLineAttributes(display, ring->progress_ring.track_gc, thickness,
                           LineSolid, CapButt, JoinMiter);
    }
    if (ring->progress_ring.fill_gc) {
        XSetForeground(display, ring->progress_ring.fill_gc,
                       ring->primitive.foreground);
        XSetLineAttributes(display, ring->progress_ring.fill_gc, thickness,
                           LineSolid, CapButt, JoinMiter);
    }
    if (ring->progress_ring.text_gc)
        XSetForeground(display, ring->progress_ring.text_gc,
                       ring->primitive.foreground);
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmProgressRingWidget ring = (XmProgressRingWidget)widget;
    Widget request_widget = request;
    Display *display = XtDisplay(widget);
    XGCValues values;
    XmFontList defaults;
    Dimension label_width = 0, label_height = 0, diameter;
    unsigned long mask = GCForeground | GCBackground;
    XmString label_copy;
    (void)args; (void)count;
    Normalize(ring);
    label_copy = ring->progress_ring.label
        ? XmStringCopy(ring->progress_ring.label) : NULL;
    ring->progress_ring.label = label_copy
        ? label_copy : XmStringCreateLocalized("");
    defaults = XmeGetDefaultRenderTable(widget, XmTEXT_FONTLIST);
    ring->progress_ring.font = defaults ? XmFontListCopy(defaults) : NULL;
    if (ring->progress_ring.font && ring->progress_ring.label)
        XmStringExtent(ring->progress_ring.font, ring->progress_ring.label,
                       &label_width, &label_height);
    diameter = _XmDpiDefaultDimension(widget, 64);
    if (diameter < 32) diameter = 32;
    ring->progress_ring.height_default = (request_widget->core.height == 0);
    ring->progress_ring.width_default = (request_widget->core.width == 0);
    if (ring->progress_ring.height_default)
        ring->core.height = ring->progress_ring.default_height =
            bounded_dimension((unsigned long)diameter +
                              2UL * ring->progress_ring.spacing);
    if (ring->progress_ring.width_default)
        ring->core.width = ring->progress_ring.default_width =
            bounded_dimension((unsigned long)diameter + label_width +
                              3UL * ring->progress_ring.spacing + 8);
    values.foreground = ring->primitive.bottom_shadow_color;
    values.background = ring->core.background_pixel;
    ring->progress_ring.track_gc = XCreateGC(display, DefaultRootWindow(display),
                                             mask, &values);
    values.foreground = ring->primitive.foreground;
    ring->progress_ring.fill_gc = XCreateGC(display, DefaultRootWindow(display),
                                            mask, &values);
    ring->progress_ring.text_gc = XCreateGC(display, DefaultRootWindow(display),
                                            mask, &values);
    if (!ring->progress_ring.track_gc || !ring->progress_ring.fill_gc ||
        !ring->progress_ring.text_gc) {
        if (ring->progress_ring.track_gc)
            XFreeGC(display, ring->progress_ring.track_gc);
        if (ring->progress_ring.fill_gc)
            XFreeGC(display, ring->progress_ring.fill_gc);
        if (ring->progress_ring.text_gc)
            XFreeGC(display, ring->progress_ring.text_gc);
        ring->progress_ring.track_gc = NULL;
        ring->progress_ring.fill_gc = NULL;
        ring->progress_ring.text_gc = NULL;
    }
    (void)_XmRegisterScaleChange(widget, ScaleChanged, NULL);
    (void)label_height;
}

static void Destroy(Widget widget) {
    XmProgressRingWidget ring = (XmProgressRingWidget)widget;
    Display *display = XtDisplay(widget);
    if (ring->progress_ring.track_gc) XFreeGC(display, ring->progress_ring.track_gc);
    if (ring->progress_ring.fill_gc) XFreeGC(display, ring->progress_ring.fill_gc);
    if (ring->progress_ring.text_gc) XFreeGC(display, ring->progress_ring.text_gc);
    if (ring->progress_ring.label) XmStringFree(ring->progress_ring.label);
    if (ring->progress_ring.font) XmFontListFree(ring->progress_ring.font);
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmProgressRingWidget ring = (XmProgressRingWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    Dimension label_width = 0, label_height = 0, diameter;
    Position center_x, center_y;
    XmString text_string;
    char text[64];
    GC text_gc;
    int fill_angle;
    (void)event; (void)region;
    if (!window || !ring->core.width || !ring->core.height) return;
    UpdateGCs(ring);
    if (!ring->progress_ring.track_gc || !ring->progress_ring.fill_gc) return;
    XClearWindow(display, window);
    if (ring->progress_ring.font && ring->progress_ring.label)
        XmStringExtent(ring->progress_ring.font, ring->progress_ring.label,
                       &label_width, &label_height);
    diameter = ring->core.height > 2 * ring->progress_ring.spacing
        ? ring->core.height - 2 * ring->progress_ring.spacing : 16;
    if (diameter > ring->core.width) diameter = ring->core.width;
    if (diameter < 16 && ring->core.width >= 16) diameter = 16;
    center_x = (Position)(ring->progress_ring.spacing + diameter / 2);
    center_y = (Position)(ring->core.height / 2);
    XDrawArc(display, window, ring->progress_ring.track_gc,
             center_x - (Position)diameter / 2,
             center_y - (Position)diameter / 2, diameter, diameter, 0, 360 * 64);
    if (ring->progress_ring.indeterminate) {
        fill_angle = 120 * 64;
    } else if (ring->progress_ring.maximum == ring->progress_ring.minimum) {
        fill_angle = ring->progress_ring.value >= ring->progress_ring.maximum
            ? 360 * 64 : 0;
    } else {
        long long numerator = (long long)ring->progress_ring.value -
                              ring->progress_ring.minimum;
        long long denominator = (long long)ring->progress_ring.maximum -
                                ring->progress_ring.minimum;
        fill_angle = (int)((numerator * 360 * 64) / denominator);
    }
    if (fill_angle)
        XDrawArc(display, window, ring->progress_ring.fill_gc,
                 center_x - (Position)diameter / 2,
                 center_y - (Position)diameter / 2, diameter, diameter,
                 90 * 64, -fill_angle);
    text_gc = XtIsSensitive(widget) && ring->progress_ring.text_gc
        ? ring->progress_ring.text_gc : ring->progress_ring.track_gc;
    if (ring->progress_ring.text_visible && ring->progress_ring.font) {
        if (ring->progress_ring.indeterminate)
            snprintf(text, sizeof(text), "…");
        else if (ring->progress_ring.maximum == ring->progress_ring.minimum)
            snprintf(text, sizeof(text), "%s", ring->progress_ring.value >=
                     ring->progress_ring.maximum ? "100%" : "0%");
        else
            snprintf(text, sizeof(text), "%d%%", (int)((100LL *
                     ((long long)ring->progress_ring.value -
                      ring->progress_ring.minimum)) /
                     ((long long)ring->progress_ring.maximum -
                      ring->progress_ring.minimum)));
        text_string = XmStringCreateLocalized(text);
        if (text_string) {
            Dimension text_width = 0, text_height = 0;
            XmStringExtent(ring->progress_ring.font, text_string, &text_width,
                           &text_height);
            XmStringDraw(display, window, ring->progress_ring.font, text_string,
                         text_gc, center_x - (Position)text_width / 2,
                         center_y - (Position)text_height / 2, text_width,
                         XmALIGNMENT_BEGINNING,
                         XmPrim_layout_direction(ring), NULL);
            XmStringFree(text_string);
        }
    }
    if (ring->progress_ring.label && ring->progress_ring.font && label_width) {
        XmStringDraw(display, window, ring->progress_ring.font,
                     ring->progress_ring.label, text_gc,
                     (Position)(ring->progress_ring.spacing + diameter + 8),
                     (Position)((ring->core.height - label_height) / 2),
                     label_width, XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(ring), NULL);
    }
    if (ring->primitive.highlighted && ring->primitive.highlight_thickness &&
        ring->core.width > 2 * ring->primitive.highlight_thickness &&
        ring->core.height > 2 * ring->primitive.highlight_thickness)
        XDrawRectangle(display, window, ring->progress_ring.fill_gc,
                       ring->primitive.highlight_thickness - 1,
                       ring->primitive.highlight_thickness - 1,
                       ring->core.width - 2 * ring->primitive.highlight_thickness,
                       ring->core.height - 2 * ring->primitive.highlight_thickness);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmProgressRingWidget old = (XmProgressRingWidget)old_widget;
    XmProgressRingWidget ring = (XmProgressRingWidget)widget;
    Boolean redraw = False;
    (void)request;
    for (Cardinal i = 0; i < *count; ++i) {
        if (strcmp(args[i].name, XtNwidth) == 0)
            ring->progress_ring.width_default = False;
        else if (strcmp(args[i].name, XtNheight) == 0)
            ring->progress_ring.height_default = False;
    }
    Normalize(ring);
    if (old->progress_ring.label != ring->progress_ring.label) {
        XmString replacement = ring->progress_ring.label
            ? XmStringCopy(ring->progress_ring.label) : NULL;
        if (!ring->progress_ring.label || replacement) {
            if (old->progress_ring.label) XmStringFree(old->progress_ring.label);
            ring->progress_ring.label = replacement;
            redraw = True;
        } else {
            ring->progress_ring.label = old->progress_ring.label;
        }
    }
    if (old->progress_ring.minimum != ring->progress_ring.minimum ||
        old->progress_ring.maximum != ring->progress_ring.maximum ||
        old->progress_ring.value != ring->progress_ring.value ||
        old->progress_ring.indeterminate != ring->progress_ring.indeterminate ||
        old->progress_ring.text_visible != ring->progress_ring.text_visible ||
        old->primitive.foreground != ring->primitive.foreground ||
        old->core.background_pixel != ring->core.background_pixel ||
        old->primitive.highlighted != ring->primitive.highlighted)
        redraw = True;
    return redraw;
}

static Dimension RingDiameter(double scale)
{
    Dimension diameter = _XmDpiScaleDimension(scale, 64);
    return diameter < 32 ? 32 : diameter;
}

static void ScaleChanged(Widget widget, double old_scale, double new_scale,
                         XtPointer closure)
{
    XmProgressRingWidget ring = (XmProgressRingWidget)widget;
    Dimension old_diameter, new_diameter;
    Dimension width, height;
    (void)closure;
    old_diameter = RingDiameter(old_scale);
    new_diameter = RingDiameter(new_scale);
    if (old_diameter == new_diameter)
        return;
    width = ring->core.width;
    height = ring->core.height;
    if (ring->progress_ring.width_default) {
        ring->progress_ring.default_width =
            (Dimension)((int)ring->progress_ring.default_width +
                        (int)new_diameter - (int)old_diameter);
        width = ring->progress_ring.default_width;
    }
    if (ring->progress_ring.height_default) {
        ring->progress_ring.default_height =
            (Dimension)((int)ring->progress_ring.default_height +
                        (int)new_diameter - (int)old_diameter);
        height = ring->progress_ring.default_height;
    }
    if (width != ring->core.width || height != ring->core.height)
        XtResizeWidget(widget, width, height, ring->core.border_width);
    if (XtIsRealized(widget))
        XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True);
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmProgressRingWidget ring = (XmProgressRingWidget)widget;
    Dimension label_width = 0, label_height = 0;
    Dimension diameter = _XmDpiDefaultDimension(widget, 64);
    (void)request;
    if (ring->progress_ring.font && ring->progress_ring.label)
        XmStringExtent(ring->progress_ring.font, ring->progress_ring.label,
                       &label_width, &label_height);
    if (diameter < 32) diameter = 32;
    reply->request_mode = CWWidth | CWHeight;
    reply->width = bounded_dimension((unsigned long)diameter + label_width +
                                     3UL * ring->progress_ring.spacing + 8);
    reply->height = bounded_dimension((unsigned long)diameter +
                                      2UL * ring->progress_ring.spacing);
    return (widget->core.width == reply->width && widget->core.height == reply->height)
               ? XtGeometryYes : XtGeometryAlmost;
}

static void Notify(XmProgressRingWidget ring, XEvent *event) {
    XmProgressBarCallbackStruct callback;
    callback.reason = XmCR_VALUE_CHANGED;
    callback.event = event;
    callback.minimum = ring->progress_ring.minimum;
    callback.maximum = ring->progress_ring.maximum;
    callback.value = ring->progress_ring.value;
    callback.indeterminate = ring->progress_ring.indeterminate;
    XtCallCallbacks((Widget)ring, XmNprogressValueChangedCallback, &callback);
}

void XmProgressRingGetRange(Widget widget, int *minimum, int *maximum) {
    XmProgressRingWidget ring;
    if (minimum) *minimum = 0;
    if (maximum) *maximum = 0;
    if (!XmIsProgressRing(widget)) return;
    ring = (XmProgressRingWidget)widget;
    if (minimum) *minimum = ring->progress_ring.minimum;
    if (maximum) *maximum = ring->progress_ring.maximum;
}
int XmProgressRingGetValue(Widget widget) {
    return XmIsProgressRing(widget)
        ? ((XmProgressRingWidget)widget)->progress_ring.value : 0;
}
void XmProgressRingSetRange(Widget widget, int minimum, int maximum,
                            Boolean notify) {
    XmProgressRingWidget ring;
    int old_min, old_max, old_value;
    if (!XmIsProgressRing(widget)) return;
    ring = (XmProgressRingWidget)widget;
    old_min = ring->progress_ring.minimum;
    old_max = ring->progress_ring.maximum;
    old_value = ring->progress_ring.value;
    ring->progress_ring.minimum = minimum;
    ring->progress_ring.maximum = maximum;
    Normalize(ring);
    if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget),
                                         0, 0, 0, 0, True);
    if (notify && (old_min != ring->progress_ring.minimum ||
                   old_max != ring->progress_ring.maximum ||
                   old_value != ring->progress_ring.value))
        Notify(ring, NULL);
}
void XmProgressRingSetValue(Widget widget, int value, Boolean notify) {
    XmProgressRingWidget ring;
    int old_value;
    if (!XmIsProgressRing(widget)) return;
    ring = (XmProgressRingWidget)widget;
    old_value = ring->progress_ring.value;
    ring->progress_ring.value = value;
    Normalize(ring);
    if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget),
                                         0, 0, 0, 0, True);
    if (notify && old_value != ring->progress_ring.value) Notify(ring, NULL);
}
Boolean XmProgressRingGetIndeterminate(Widget widget) {
    return XmIsProgressRing(widget)
        ? ((XmProgressRingWidget)widget)->progress_ring.indeterminate : False;
}
void XmProgressRingSetIndeterminate(Widget widget, Boolean indeterminate,
                                    Boolean notify) {
    XmProgressRingWidget ring;
    Boolean old;
    if (!XmIsProgressRing(widget)) return;
    ring = (XmProgressRingWidget)widget;
    old = ring->progress_ring.indeterminate;
    ring->progress_ring.indeterminate = indeterminate ? True : False;
    if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget),
                                         0, 0, 0, 0, True);
    if (notify && old != ring->progress_ring.indeterminate)
        Notify(ring, NULL);
}

Widget XmCreateProgressRing(Widget parent, char *name, ArgList args,
                            Cardinal arg_count) {
    return XtCreateWidget(name, xmProgressRingWidgetClass, parent, args,
                          arg_count);
}
Widget XmVaCreateProgressRing(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name,
        xmProgressRingWidgetClass, parent, False, var, count); va_end(var);
    return widget;
}
Widget XmVaCreateManagedProgressRing(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name,
        xmProgressRingWidgetClass, parent, True, var, count); va_end(var);
    return widget;
}
