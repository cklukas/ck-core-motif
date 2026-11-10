/* Additive bounded Xt-timer spinner control.  No worker thread is used. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "PaintI.h"

#include <stdio.h>
#include <limits.h>
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/SpinnerP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "XmI.h"
#include <Xm/Appearance.h>
#include "AppearanceDrawI.h"
#include <Xm/DrawP.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void BufferedRedisplay(Widget, XEvent *, Region);
static Boolean BufferedSetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);
static void Events(Widget, XtPointer, XEvent *, Boolean *);
static void Tick(XtPointer, XtIntervalId *);

static void PreferredSize(Widget, double, Dimension *, Dimension *);
static void ScaleChanged(Widget, double, double, XtPointer);

static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmSpinnerWidget spinner = (XmSpinnerWidget)widget;
    (void)offset;
    if (spinner->spinner.check_set_render_table) value->addr = NULL;
    else {
        spinner->spinner.check_set_render_table = True;
        value->addr = (char *)&spinner->spinner.font;
    }
}

#define Offset(field) XtOffsetOf(XmSpinnerRec, field)
static XtResource resources[] = {
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
     Offset(spinner.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable),
     Offset(spinner.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNspinnerActive, XmCSpinnerActive, XmRBoolean, sizeof(Boolean),
     Offset(spinner.active), XmRImmediate, (XtPointer)False},
    {XmNspinnerInterval, XmCSpinnerInterval, XmRInt, sizeof(int),
     Offset(spinner.interval), XmRImmediate, (XtPointer)100},
    {XmNspinnerLabelString, XmCSpinnerLabelString, XmRXmString,
     sizeof(XmString), Offset(spinner.label), XmRImmediate, (XtPointer)NULL},
    {XmNspinnerStateChangedCallback, XmCSpinnerStateChangedCallback,
     XmRCallback, sizeof(XtCallbackList),
     Offset(spinner.state_changed_callback), XmRPointer, (XtPointer)NULL},
    {XmNspacing, XmCSpacing, XmRHorizontalDimension, sizeof(Dimension),
     Offset(spinner.spacing), XmRImmediate, (XtPointer)8},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};

externaldef(xmspinnerclassrec) XmSpinnerClassRec xmSpinnerClassRec = {
    {
        (WidgetClass)&xmPrimitiveClassRec, "XmSpinner", sizeof(XmSpinnerRec),
        NULL, NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0,
        resources, XtNumber(resources), NULLQUARK, True,
        XtExposeCompressMaximal, True, False, Destroy, NULL, BufferedRedisplay,
        BufferedSetValues, NULL, XtInheritSetValuesAlmost, NULL, NULL, XtVersion,
        NULL, XtInheritTranslations, QueryGeometry, NULL, NULL
    },
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
     XtInheritTranslations, NULL, NULL, 0, NULL},
    {NULL}
};
externaldef(xmspinnerwidgetclass) WidgetClass xmSpinnerWidgetClass =
    (WidgetClass)&xmSpinnerClassRec;

static int bounded_interval(int interval) {
    if (interval < 10) return 10;
    if (interval > 1000) return 1000;
    return interval;
}

static Dimension bounded_dimension(size_t value) {
    const size_t maximum = (size_t)(Dimension)~(Dimension)0;
    return (Dimension)(value > maximum ? maximum : value);
}

static Position centered_offset(Dimension container, Dimension content) {
    /* Keep oversized labels negative instead of wrapping Dimension arithmetic. */
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

static Boolean eligible(XmSpinnerWidget spinner) {
    XWindowAttributes attributes;
    Widget widget = (Widget)spinner;
    if (!spinner->spinner.active || !XtIsRealized(widget) ||
        !XtIsManaged(widget) || !XtIsSensitive(widget) || !XtWindow(widget))
        return False;
    return XGetWindowAttributes(XtDisplay(widget), XtWindow(widget),
                                &attributes) && attributes.map_state == IsViewable;
}

static void SyncTimer(XmSpinnerWidget spinner) {
    Widget widget = (Widget)spinner;
    if (!eligible(spinner)) {
        if (spinner->spinner.timer_pending) {
            XtRemoveTimeOut(spinner->spinner.timer);
            spinner->spinner.timer_pending = False;
        }
        return;
    }
    if (!spinner->spinner.timer_pending) {
        spinner->spinner.timer = XtAppAddTimeOut(
            XtWidgetToApplicationContext(widget),
            (unsigned long)bounded_interval(spinner->spinner.interval), Tick,
            (XtPointer)spinner);
        spinner->spinner.timer_pending = True;
    }
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmSpinnerWidget spinner = (XmSpinnerWidget)widget;
    Display *display = XtDisplay(widget);
    XGCValues values;
    XmFontList defaults;
    XmString replacement;
    Dimension preferred_width, preferred_height;
    double scale = 1.0;
    unsigned long mask = GCForeground | GCBackground;
    (void)request; (void)args; (void)count;
    spinner->spinner.interval = bounded_interval(spinner->spinner.interval);
    if (spinner->spinner.label)
    {
        replacement = XmStringCopy(spinner->spinner.label);
        spinner->spinner.label = replacement != NULL
            ? replacement : XmStringCreateLocalized("");
    }
    else
        spinner->spinner.label = XmStringCreateLocalized("");
    defaults = spinner->spinner.font ? spinner->spinner.font :
        XmeGetDefaultRenderTable(widget, XmTEXT_FONTLIST);
    spinner->spinner.font = defaults ? XmFontListCopy(defaults) : NULL;
    spinner->spinner.width_default = (request->core.width == 0);
    spinner->spinner.height_default = (request->core.height == 0);
    _XmGetWidgetDpiScale(widget, &scale);
    PreferredSize(widget, scale, &preferred_width, &preferred_height);
    if (spinner->spinner.width_default) spinner->core.width = preferred_width;
    if (spinner->spinner.height_default) spinner->core.height = preferred_height;
    values.foreground = spinner->primitive.foreground;
    values.background = spinner->core.background_pixel;
    spinner->spinner.ring_gc = XCreateGC(display, DefaultRootWindow(display),
                                         mask, &values);
    spinner->spinner.text_gc = XCreateGC(display, DefaultRootWindow(display),
                                         mask, &values);
    (void)_XmRegisterScaleChange(widget, ScaleChanged, NULL);
    XtAddEventHandler(widget, StructureNotifyMask | VisibilityChangeMask,
                      False, Events, NULL);
}

static void Destroy(Widget widget) {
    XmSpinnerWidget spinner = (XmSpinnerWidget)widget;
    Display *display = XtDisplay(widget);
    if (spinner->spinner.timer_pending) {
        XtRemoveTimeOut(spinner->spinner.timer);
        spinner->spinner.timer_pending = False;
    }
    XtRemoveEventHandler(widget, StructureNotifyMask | VisibilityChangeMask,
                         False, Events, NULL);
    if (spinner->spinner.ring_gc) XFreeGC(display, spinner->spinner.ring_gc);
    if (spinner->spinner.text_gc) XFreeGC(display, spinner->spinner.text_gc);
    if (spinner->spinner.label) XmStringFree(spinner->spinner.label);
    if (spinner->spinner.font) XmFontListFree(spinner->spinner.font);
}

static void UpdateGCs(XmSpinnerWidget spinner) {
    Display *display = XtDisplay((Widget)spinner);
    unsigned char role=XmAPPEARANCE_ROLE_NORMAL;XtVaGetValues((Widget)spinner,XmNappearanceRole,&role,NULL);
    if (spinner->spinner.ring_gc) {
        XSetBackground(display, spinner->spinner.ring_gc,
                       spinner->core.background_pixel);
        XSetForeground(display, spinner->spinner.ring_gc,
                       !XtIsSensitive((Widget)spinner) ? spinner->primitive.bottom_shadow_color :
                       ((_XmAppearanceGeometryProfile(display)==XmAPPEARANCE_MODERN_LIGHT ||
                         _XmAppearanceGeometryProfile(display)==XmAPPEARANCE_MODERN_DARK) &&
                        role==XmAPPEARANCE_ROLE_NORMAL && _XmAppearanceOwnsResource((Widget)spinner,XmNforeground) ?
                        spinner->primitive.highlight_color : spinner->primitive.foreground));
    }
    if (spinner->spinner.text_gc) {
        XSetBackground(display, spinner->spinner.text_gc,
                       spinner->core.background_pixel);
        XSetForeground(display, spinner->spinner.text_gc,
                       spinner->primitive.foreground);
    }
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmSpinnerWidget spinner = (XmSpinnerWidget)widget;
    Display *display = XtDisplay(widget);
    Drawable window = _XmPaintDrawable(widget);
    Dimension label_width = 0, label_height = 0;
    int diameter, ring_x, ring_y, thickness;
    GC gc;
    int start;
    (void)event; (void)region;
    if (!window || !spinner->core.width || !spinner->core.height) return;
    UpdateGCs(spinner);
    gc = spinner->spinner.ring_gc ? spinner->spinner.ring_gc
                                  : spinner->spinner.text_gc;
    if (!gc) return;

    if (spinner->spinner.font && spinner->spinner.label)
        XmStringExtent(spinner->spinner.font, spinner->spinner.label,
                       &label_width, &label_height);
    _XmAppearanceRingBounds(spinner->core.width, spinner->core.height,
        spinner->spinner.spacing, spinner->primitive.highlight_thickness,
        _XmDpiDefaultDimension(widget,32), &ring_x, &ring_y, &diameter);
    if (!label_width) ring_x = ((int)spinner->core.width - diameter) / 2;
    else if (XmDirectionMatchPartial(XmPrim_layout_direction(spinner),
                                     XmRIGHT_TO_LEFT, XmHORIZONTAL_MASK))
        ring_x = (int)spinner->core.width - ring_x - diameter;
    thickness = MAX(1, (int)_XmDpiDefaultDimension(widget,3));
    start = (int)((spinner->spinner.phase % 12) * 30) * 64;
    _XmAppearanceDrawRing(display, window, gc, ring_x, ring_y,
                          diameter, thickness, start, 250 * 64);
    if (spinner->spinner.font && spinner->spinner.label) {
        XmStringDraw(display, window, spinner->spinner.font,
                     spinner->spinner.label, gc,
                     bounded_position(XmDirectionMatchPartial(XmPrim_layout_direction(spinner),
                                                               XmRIGHT_TO_LEFT,
                                                               XmHORIZONTAL_MASK)
                                      ? (long long)ring_x - _XmDpiDefaultDimension(widget,8) - label_width
                                      : (long long)ring_x + diameter + _XmDpiDefaultDimension(widget,8)),
                     centered_offset(spinner->core.height, label_height),
                     label_width, XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(spinner), NULL);
    }
    if (spinner->primitive.highlighted && spinner->primitive.highlight_thickness &&
        spinner->core.width > 2 * spinner->primitive.highlight_thickness &&
        spinner->core.height > 2 * spinner->primitive.highlight_thickness)
        XmeDrawHighlight(display, window, spinner->primitive.highlight_GC,
                         0, 0, spinner->core.width, spinner->core.height,
                         spinner->primitive.highlight_thickness);
}

static void BufferedRedisplay(Widget widget, XEvent *event, Region region)
{
    _XmBufferedPaint(widget, Redisplay, event, region);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmSpinnerWidget old = (XmSpinnerWidget)old_widget;
    XmSpinnerWidget spinner = (XmSpinnerWidget)widget;
    Boolean redraw = False;
    Boolean geometry_changed = False;
    XmString replacement;
    (void)request; (void)args; (void)count;
    for (Cardinal i = 0; i < *count; ++i) {
        if (strcmp(args[i].name, XtNwidth) == 0) {
            Boolean automatic = (Dimension)args[i].value == 0;
            geometry_changed |= spinner->spinner.width_default != automatic;
            spinner->spinner.width_default = automatic;
        } else if (strcmp(args[i].name, XtNheight) == 0) {
            Boolean automatic = (Dimension)args[i].value == 0;
            geometry_changed |= spinner->spinner.height_default != automatic;
            spinner->spinner.height_default = automatic;
        }
    }
    spinner->spinner.interval = bounded_interval(spinner->spinner.interval);
    if (old->spinner.interval != spinner->spinner.interval &&
        spinner->spinner.timer_pending) {
        XtRemoveTimeOut(spinner->spinner.timer);
        spinner->spinner.timer_pending = False;
    }
    if (old->spinner.font != spinner->spinner.font) {
        XmFontList source = spinner->spinner.font ? spinner->spinner.font :
            XmeGetDefaultRenderTable(widget, XmTEXT_FONTLIST);
        XmFontList replacement_font = source ? XmFontListCopy(source) : NULL;
        if (replacement_font) {
            if (old->spinner.font) XmFontListFree(old->spinner.font);
            spinner->spinner.font = replacement_font;
            redraw = True;
        } else spinner->spinner.font = old->spinner.font;
    }
    if (old->spinner.label != spinner->spinner.label) {
        replacement = spinner->spinner.label != NULL
            ? XmStringCopy(spinner->spinner.label) : NULL;
        if (spinner->spinner.label == NULL || replacement != NULL) {
            if (old->spinner.label) XmStringFree(old->spinner.label);
            spinner->spinner.label = replacement;
            redraw = True;
        } else {
            spinner->spinner.label = old->spinner.label;
        }
    }
    if (old->spinner.active != spinner->spinner.active) {
        redraw = True;
        SyncTimer(spinner);
    }
    else if (old->spinner.interval != spinner->spinner.interval)
        SyncTimer(spinner);
    if (old->spinner.interval != spinner->spinner.interval ||
        old->primitive.foreground != spinner->primitive.foreground ||
        old->core.background_pixel != spinner->core.background_pixel ||
        old->primitive.highlighted != spinner->primitive.highlighted ||
        XmPrim_layout_direction(old) != XmPrim_layout_direction(spinner))
        redraw = True;
    if (old->spinner.spacing != spinner->spinner.spacing) redraw = True;
    if (redraw || geometry_changed) {
        Dimension width, height;
        double scale = 1.0;
        _XmGetWidgetDpiScale(widget, &scale);
        PreferredSize(widget, scale, &width, &height);
        if (spinner->spinner.width_default) spinner->core.width = width;
        if (spinner->spinner.height_default) spinner->core.height = height;
    }
    return redraw || geometry_changed;
}

static void PreferredSize(Widget widget, double scale, Dimension *width, Dimension *height)
{
    XmSpinnerWidget spinner = (XmSpinnerWidget)widget;
    Dimension label_width = 0, label_height = 0;
    unsigned long diameter = _XmDpiScaleDimension(scale,32);
    unsigned long padding = 2UL * MAX(spinner->spinner.spacing,
                                      spinner->primitive.highlight_thickness);
    if (spinner->spinner.font && spinner->spinner.label)
        XmStringExtent(spinner->spinner.font, spinner->spinner.label, &label_width, &label_height);
    *width = bounded_dimension(diameter + padding + (label_width
        ? (unsigned long)label_width + _XmDpiScaleDimension(scale,8) : 0));
    *height = bounded_dimension(MAX(diameter,label_height) + padding);
}

static void ScaleChanged(Widget widget, double old_scale, double new_scale, XtPointer closure)
{
    XmSpinnerWidget spinner = (XmSpinnerWidget)widget;
    Dimension width, height, offered_width, offered_height;
    XtGeometryResult result;
    (void)old_scale; (void)closure;
    PreferredSize(widget,new_scale,&width,&height);
    if (!spinner->spinner.width_default) width = spinner->core.width;
    if (!spinner->spinner.height_default) height = spinner->core.height;
    if (width != spinner->core.width || height != spinner->core.height) {
        result = XtMakeResizeRequest(widget,width,height,&offered_width,&offered_height);
        if (result == XtGeometryAlmost &&
            (spinner->spinner.width_default || offered_width == width) &&
            (spinner->spinner.height_default || offered_height == height))
            (void)XtMakeResizeRequest(widget,offered_width,offered_height,NULL,NULL);
    }
    if (XtIsRealized(widget)) _XmPaintInvalidate(widget);
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply)
{
    XmSpinnerWidget spinner = (XmSpinnerWidget)widget;
    double scale = 1.0;
    _XmGetWidgetDpiScale(widget,&scale);
    reply->request_mode = CWWidth | CWHeight;
    PreferredSize(widget,scale,&reply->width,&reply->height);
    if (!spinner->spinner.width_default)
        reply->width = spinner->core.width;
    if (!spinner->spinner.height_default)
        reply->height = spinner->core.height;
    return XmeReplyToQueryGeometry(widget,request,reply);
}

static void Events(Widget widget, XtPointer data, XEvent *event,
                   Boolean *dispatch) {
    (void)data; (void)dispatch;
    if (widget == NULL || event == NULL || !XmIsSpinner(widget))
	return;
    if (event->type == MapNotify || event->type == UnmapNotify ||
        event->type == VisibilityNotify)
        SyncTimer((XmSpinnerWidget)widget);
}

static void Tick(XtPointer data, XtIntervalId *id) {
    XmSpinnerWidget spinner = (XmSpinnerWidget)data;
    (void)id;
    if (spinner == NULL)
	return;
    if (spinner->core.being_destroyed)
	return;
    spinner->spinner.timer_pending = False;
    if (!eligible(spinner)) return;
    spinner->spinner.phase = (spinner->spinner.phase + 1) % 12;
    if (XtIsRealized((Widget)spinner))
        _XmPaintInvalidate((Widget)spinner);
    SyncTimer(spinner);
}

static void Notify(XmSpinnerWidget spinner, XEvent *event) {
    XmSpinnerCallbackStruct callback;
    if (spinner == NULL || spinner->core.being_destroyed)
        return;
    callback.reason = XmCR_VALUE_CHANGED;
    callback.event = event;
    callback.active = spinner->spinner.active;
    callback.phase = spinner->spinner.phase;
    XtCallCallbacks((Widget)spinner, XmNspinnerStateChangedCallback,
                    &callback);
}

Boolean XmSpinnerGetActive(Widget widget) {
    return widget != NULL && XmIsSpinner(widget) && !widget->core.being_destroyed
        ? ((XmSpinnerWidget)widget)->spinner.active : False;
}
void XmSpinnerSetActive(Widget widget, Boolean active, Boolean notify) {
    XmSpinnerWidget spinner;
    Boolean old;
    if (widget == NULL || !XmIsSpinner(widget) || widget->core.being_destroyed)
        return;
    spinner = (XmSpinnerWidget)widget;
    old = spinner->spinner.active;
    spinner->spinner.active = active ? True : False;
    SyncTimer(spinner);
    if (XtIsRealized(widget)) _XmPaintInvalidate(widget);
    if (notify && old != spinner->spinner.active)
        Notify(spinner, NULL);
}
unsigned int XmSpinnerGetPhase(Widget widget) {
    return widget != NULL && XmIsSpinner(widget) && !widget->core.being_destroyed
        ? ((XmSpinnerWidget)widget)->spinner.phase : 0;
}

Widget XmCreateSpinner(Widget parent, char *name, ArgList args,
                       Cardinal arg_count) {
    return XtCreateWidget(name, xmSpinnerWidgetClass, parent, args, arg_count);
}
Widget XmVaCreateSpinner(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmSpinnerWidgetClass,
        parent, False, var, count); va_end(var);
    return widget;
}
Widget XmVaCreateManagedSpinner(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmSpinnerWidgetClass,
        parent, True, var, count); va_end(var);
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
