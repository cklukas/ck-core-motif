/* Additive compact progress ring.  It shares the ProgressBar value contract. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "PaintI.h"

#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <X11/IntrinsicP.h>
#include <Xm/ProgressRingP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "XmI.h"
#include "AppearanceDrawI.h"
#include <Xm/DrawP.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void BufferedRedisplay(Widget, XEvent *, Region);
static Boolean BufferedSetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);

static void PreferredSize(Widget, double, Dimension *, Dimension *);
static Dimension RingDiameter(Widget, double);
static void ScaleChanged(Widget, double, double, XtPointer);

static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmProgressRingWidget ring = (XmProgressRingWidget)widget;
    (void)offset;
    if (ring->progress_ring.check_set_render_table) value->addr = NULL;
    else {
        ring->progress_ring.check_set_render_table = True;
        value->addr = (char *)&ring->progress_ring.font;
    }
}

#define Offset(field) XtOffsetOf(XmProgressRingRec, field)
static XtResource resources[] = {
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
     Offset(progress_ring.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable),
     Offset(progress_ring.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
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
            NULL, BufferedRedisplay, BufferedSetValues, NULL, XtInheritSetValuesAlmost, NULL,
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

static Position centered_offset(Dimension container, Dimension content) {
    /* Subtract in a signed type so an oversized label is not wrapped as an
     * unsigned Dimension before it is converted to an X coordinate. */
    long long offset = ((long long)container - (long long)content) / 2;
    if (offset < SHRT_MIN) return SHRT_MIN;
    if (offset > SHRT_MAX) return SHRT_MAX;
    return (Position)offset;
}

static Position bounded_position(long long value) {
    if (value < SHRT_MIN) return SHRT_MIN;
    if (value > SHRT_MAX) return SHRT_MAX;
    return (Position)value;
}

static void UpdateGCs(XmProgressRingWidget ring) {
    Display *display = XtDisplay((Widget)ring);
    if (ring->progress_ring.track_gc) {
        XSetBackground(display, ring->progress_ring.track_gc,
                       ring->core.background_pixel);
        XSetForeground(display, ring->progress_ring.track_gc,
                       ring->primitive.bottom_shadow_color);
    }
    if (ring->progress_ring.fill_gc) {
        XSetBackground(display, ring->progress_ring.fill_gc,
                       ring->core.background_pixel);
        XSetForeground(display, ring->progress_ring.fill_gc,
                       ring->primitive.foreground);
    }
    if (ring->progress_ring.text_gc) {
        XSetBackground(display, ring->progress_ring.text_gc,
                       ring->core.background_pixel);
        XSetForeground(display, ring->progress_ring.text_gc,
                       ring->primitive.foreground);
    }
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmProgressRingWidget ring = (XmProgressRingWidget)widget;
    Display *display = XtDisplay(widget);
    XGCValues values;
    XmFontList defaults;
    Dimension preferred_width, preferred_height;
    double scale = 1.0;
    unsigned long mask = GCForeground | GCBackground;
    XmString label_copy;
    (void)args; (void)count;
    Normalize(ring);
    label_copy = ring->progress_ring.label
        ? XmStringCopy(ring->progress_ring.label) : NULL;
    ring->progress_ring.label = label_copy
        ? label_copy : XmStringCreateLocalized("");
    defaults = ring->progress_ring.font ? ring->progress_ring.font :
        XmeGetDefaultRenderTable(widget, XmTEXT_FONTLIST);
    ring->progress_ring.font = defaults ? XmFontListCopy(defaults) : NULL;
    ring->progress_ring.width_default = (request->core.width == 0);
    ring->progress_ring.height_default = (request->core.height == 0);
    _XmGetWidgetDpiScale(widget, &scale);
    PreferredSize(widget, scale, &preferred_width, &preferred_height);
    if (ring->progress_ring.width_default) ring->core.width = preferred_width;
    if (ring->progress_ring.height_default) ring->core.height = preferred_height;
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
    Drawable window = _XmPaintDrawable(widget);
    Dimension label_width = 0, label_height = 0;
    int diameter, ring_x, ring_y, thickness;
    double scale = 1.0;
    Position center_x, center_y;
    XmString text_string;
    char text[64];
    GC text_gc;
    int fill_angle;
    (void)event; (void)region;
    if (!window || !ring->core.width || !ring->core.height) return;
    UpdateGCs(ring);
    if (!ring->progress_ring.track_gc || !ring->progress_ring.fill_gc) return;

    if (ring->progress_ring.font && ring->progress_ring.label)
        XmStringExtent(ring->progress_ring.font, ring->progress_ring.label,
                       &label_width, &label_height);
    _XmGetWidgetDpiScale(widget, &scale);
    _XmAppearanceRingBounds(ring->core.width, ring->core.height,
        ring->progress_ring.spacing, ring->primitive.highlight_thickness,
        MIN(SHRT_MAX, RingDiameter(widget,scale)), &ring_x, &ring_y, &diameter);
    if (!label_width) ring_x = ((int)ring->core.width - diameter) / 2;
    thickness = MAX(1, (int)_XmDpiDefaultDimension(widget,4));
    center_x = ring_x + diameter / 2;
    center_y = ring_y + diameter / 2;
    _XmAppearanceDrawRing(display, window, ring->progress_ring.track_gc,
                          ring_x, ring_y, diameter, thickness, 0, 360 * 64);
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
        _XmAppearanceDrawRing(display, window, ring->progress_ring.fill_gc,
                              ring_x, ring_y, diameter, thickness, 90 * 64, -fill_angle);
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
            /* Fit the text rectangle inside the circular opening, including
             * its corners, rather than merely inside the opening's square. */
            if (diameter > 2 * thickness &&
                (unsigned long long)text_width * text_width +
                (unsigned long long)text_height * text_height <=
                (unsigned long long)(diameter - 2 * thickness) *
                                    (diameter - 2 * thickness))
                XmStringDraw(display, window, ring->progress_ring.font, text_string,
                         text_gc, bounded_position((long long)center_x - (long long)text_width / 2),
                         bounded_position((long long)center_y - (long long)text_height / 2), text_width,
                         XmALIGNMENT_BEGINNING,
                         XmPrim_layout_direction(ring), NULL);
            XmStringFree(text_string);
        }
    }
    if (ring->progress_ring.label && ring->progress_ring.font && label_width) {
        XmStringDraw(display, window, ring->progress_ring.font,
                     ring->progress_ring.label, text_gc,
                     bounded_position((long long)ring_x + diameter + _XmDpiDefaultDimension(widget,8)),
                     centered_offset(ring->core.height, label_height),
                     label_width, XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(ring), NULL);
    }
    if (ring->primitive.highlighted && ring->primitive.highlight_thickness &&
        ring->core.width > 2 * ring->primitive.highlight_thickness &&
        ring->core.height > 2 * ring->primitive.highlight_thickness)
        XmeDrawHighlight(display, window, ring->primitive.highlight_GC,
                         0, 0, ring->core.width, ring->core.height,
                         ring->primitive.highlight_thickness);
}

static void BufferedRedisplay(Widget widget, XEvent *event, Region region)
{
    _XmBufferedPaint(widget, Redisplay, event, region);
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
    if (old->progress_ring.font != ring->progress_ring.font) {
        XmFontList source = ring->progress_ring.font ? ring->progress_ring.font :
            XmeGetDefaultRenderTable(widget, XmTEXT_FONTLIST);
        XmFontList replacement_font = source ? XmFontListCopy(source) : NULL;
        if (replacement_font) {
            if (old->progress_ring.font) XmFontListFree(old->progress_ring.font);
            ring->progress_ring.font = replacement_font;
            redraw = True;
        } else ring->progress_ring.font = old->progress_ring.font;
    }
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
    if (old->progress_ring.spacing != ring->progress_ring.spacing) redraw = True;
    if (redraw) {
        Dimension width, height;
        double scale = 1.0;
        _XmGetWidgetDpiScale(widget, &scale);
        PreferredSize(widget, scale, &width, &height);
        if (ring->progress_ring.width_default) ring->core.width = width;
        if (ring->progress_ring.height_default) ring->core.height = height;
    }
    return redraw;
}

static Dimension RingDiameter(Widget widget, double scale)
{
    XmProgressRingWidget ring = (XmProgressRingWidget)widget;
    unsigned long diameter = _XmDpiScaleDimension(scale,64);
    if (ring->progress_ring.text_visible && ring->progress_ring.font) {
        XmString sample = XmStringCreateLocalized("100%");
        Dimension text_width = 0, text_height = 0;
        if (sample) {
            XmStringExtent(ring->progress_ring.font, sample, &text_width, &text_height);
            XmStringFree(sample);
            unsigned long content = (unsigned long)ceil(sqrt(
                (double)text_width * text_width + (double)text_height * text_height));
            content += 2UL * _XmDpiScaleDimension(scale,4) + _XmDpiScaleDimension(scale,4);
            diameter = MAX(diameter,content);
        }
    }
    return bounded_dimension(diameter);
}

static void PreferredSize(Widget widget, double scale, Dimension *width, Dimension *height)
{
    XmProgressRingWidget ring = (XmProgressRingWidget)widget;
    Dimension label_width = 0, label_height = 0;
    unsigned long diameter = RingDiameter(widget,scale);
    unsigned long padding = 2UL * MAX(ring->progress_ring.spacing,
                                      ring->primitive.highlight_thickness);
    if (ring->progress_ring.font && ring->progress_ring.label)
        XmStringExtent(ring->progress_ring.font, ring->progress_ring.label, &label_width, &label_height);
    *width = bounded_dimension(diameter + padding + (label_width
        ? (unsigned long)label_width + _XmDpiScaleDimension(scale,8) : 0));
    *height = bounded_dimension(MAX(diameter,label_height) + padding);
}

static void ScaleChanged(Widget widget, double old_scale, double new_scale, XtPointer closure)
{
    XmProgressRingWidget ring = (XmProgressRingWidget)widget;
    Dimension width, height, offered_width, offered_height;
    XtGeometryResult result;
    (void)old_scale; (void)closure;
    PreferredSize(widget,new_scale,&width,&height);
    if (!ring->progress_ring.width_default) width = ring->core.width;
    if (!ring->progress_ring.height_default) height = ring->core.height;
    if (width != ring->core.width || height != ring->core.height) {
        result = XtMakeResizeRequest(widget,width,height,&offered_width,&offered_height);
        if (result == XtGeometryAlmost &&
            (ring->progress_ring.width_default || offered_width == width) &&
            (ring->progress_ring.height_default || offered_height == height))
            (void)XtMakeResizeRequest(widget,offered_width,offered_height,NULL,NULL);
    }
    if (XtIsRealized(widget)) _XmPaintInvalidate(widget);
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply)
{
    double scale = 1.0;
    _XmGetWidgetDpiScale(widget,&scale);
    reply->request_mode = CWWidth | CWHeight;
    PreferredSize(widget,scale,&reply->width,&reply->height);
    return XmeReplyToQueryGeometry(widget,request,reply);
}

static void Notify(XmProgressRingWidget ring, XEvent *event) {
    XmProgressBarCallbackStruct callback;
    if (ring == NULL || ring->core.being_destroyed)
        return;
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
    if (widget == NULL || !XmIsProgressRing(widget) || widget->core.being_destroyed)
        return;
    ring = (XmProgressRingWidget)widget;
    if (minimum) *minimum = ring->progress_ring.minimum;
    if (maximum) *maximum = ring->progress_ring.maximum;
}
int XmProgressRingGetValue(Widget widget) {
    return widget != NULL && XmIsProgressRing(widget)
        ? ((XmProgressRingWidget)widget)->progress_ring.value : 0;
}
void XmProgressRingSetRange(Widget widget, int minimum, int maximum,
                            Boolean notify) {
    XmProgressRingWidget ring;
    int old_min, old_max, old_value;
    if (widget == NULL || !XmIsProgressRing(widget) || widget->core.being_destroyed)
        return;
    ring = (XmProgressRingWidget)widget;
    old_min = ring->progress_ring.minimum;
    old_max = ring->progress_ring.maximum;
    old_value = ring->progress_ring.value;
    ring->progress_ring.minimum = minimum;
    ring->progress_ring.maximum = maximum;
    Normalize(ring);
    if (XtIsRealized(widget)) _XmPaintInvalidate(widget);
    if (notify && (old_min != ring->progress_ring.minimum ||
                   old_max != ring->progress_ring.maximum ||
                   old_value != ring->progress_ring.value))
        Notify(ring, NULL);
}
void XmProgressRingSetValue(Widget widget, int value, Boolean notify) {
    XmProgressRingWidget ring;
    int old_value;
    if (widget == NULL || !XmIsProgressRing(widget) || widget->core.being_destroyed)
        return;
    ring = (XmProgressRingWidget)widget;
    old_value = ring->progress_ring.value;
    ring->progress_ring.value = value;
    Normalize(ring);
    if (XtIsRealized(widget)) _XmPaintInvalidate(widget);
    if (notify && old_value != ring->progress_ring.value) Notify(ring, NULL);
}
Boolean XmProgressRingGetIndeterminate(Widget widget) {
    return widget != NULL && XmIsProgressRing(widget)
        ? ((XmProgressRingWidget)widget)->progress_ring.indeterminate : False;
}
void XmProgressRingSetIndeterminate(Widget widget, Boolean indeterminate,
                                    Boolean notify) {
    XmProgressRingWidget ring;
    Boolean old;
    if (widget == NULL || !XmIsProgressRing(widget)) return;
    ring = (XmProgressRingWidget)widget;
    old = ring->progress_ring.indeterminate;
    ring->progress_ring.indeterminate = indeterminate ? True : False;
    if (XtIsRealized(widget)) _XmPaintInvalidate(widget);
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

/* Xt's default redisplay request clears the front window. Queue our complete
 * frame instead, after resource and geometry negotiation has finished. */
static Boolean BufferedSetValues(Widget old, Widget request, Widget widget,
                                ArgList args, Cardinal *count)
{
    if (SetValues(old, request, widget, args, count))
        _XmPaintInvalidate(widget);
    return False;
}
