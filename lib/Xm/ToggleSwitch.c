/*
 * Additive binary switch widget.
 *
 * This is deliberately a small XmPrimitive subclass.  Existing ToggleButton
 * widgets and their resource semantics are untouched; applications opt in by
 * creating XmToggleSwitch widgets.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <Xm/ToggleSwitchP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "TravActI.h"
#include "XmI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Toggle(Widget, XEvent *, String *, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);

static char translations[] =
    "<Key>space: ToggleSwitch()\n"
    "<Key>Return: ToggleSwitch()\n"
    "<Btn1Up>: ToggleSwitch()";

static XtActionsRec actions[] = {
    {"ToggleSwitch", Toggle},
    {"ButtonTakeFocus", _XmButtonTakeFocus}
};

#define Offset(field) XtOffsetOf(XmToggleSwitchRec, field)
static XtResource resources[] = {
    {XmNset, XmCSet, XmRBoolean, sizeof(Boolean),
     Offset(toggle_switch.set), XmRImmediate, (XtPointer)False},
    {XmNlabelString, XmCXmString, XmRXmString, sizeof(XmString),
     Offset(toggle_switch.label), XmRImmediate, (XtPointer)NULL},
    {XmNvalueChangedCallback, XmCValueChangedCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(toggle_switch.value_changed_callback),
     XmRPointer, (XtPointer)NULL},
    {XmNspacing, XmCSpacing, XmRHorizontalDimension, sizeof(Dimension),
     Offset(toggle_switch.spacing), XmRImmediate, (XtPointer)8},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};

externaldef(xmtoggleswitchclassrec) XmToggleSwitchClassRec
    xmToggleSwitchClassRec = {
    {
        (WidgetClass)&xmPrimitiveClassRec, "XmToggleSwitch",
        sizeof(XmToggleSwitchRec), NULL, NULL, False, Initialize, NULL,
        XtInheritRealize, actions, XtNumber(actions), resources,
        XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal, True,
        False, Destroy, NULL, Redisplay, SetValues, NULL,
        XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL, translations,
        QueryGeometry, NULL, NULL
    },
    {
        XmInheritBorderHighlight, XmInheritBorderUnhighlight,
        XtInheritTranslations, NULL, NULL, 0, NULL
    },
    {NULL}
};

externaldef(xmtoggleswitchwidgetclass) WidgetClass xmToggleSwitchWidgetClass =
    (WidgetClass)&xmToggleSwitchClassRec;

static void update_gcs(XmToggleSwitchWidget sw) {
    Display *display = XtDisplay((Widget)sw);
    XSetForeground(display, sw->toggle_switch.track_gc,
                   sw->toggle_switch.track_color);
    XSetForeground(display, sw->toggle_switch.knob_gc,
                   sw->toggle_switch.knob_color);
    XSetForeground(display, sw->toggle_switch.insensitive_gc,
                   sw->primitive.bottom_shadow_color);
}

static void Initialize(Widget request, Widget new_widget, ArgList args,
                       Cardinal *num_args) {
    XmToggleSwitchWidget sw = (XmToggleSwitchWidget)new_widget;
    Display *display = XtDisplay(new_widget);
    XGCValues values;
    unsigned long mask = GCForeground | GCBackground;
    XmFontList defaults;
    XmString replacement;
    Dimension label_width = 0, label_height = 0;
    (void)request; (void)args; (void)num_args;

    if (!sw->toggle_switch.label)
        sw->toggle_switch.label = XmStringCreateLocalized("");
    else
    {
        replacement = XmStringCopy(sw->toggle_switch.label);
        if (replacement != NULL)
            sw->toggle_switch.label = replacement;
        else
            sw->toggle_switch.label = XmStringCreateLocalized("");
    }
    defaults = XmeGetDefaultRenderTable(new_widget, XmBUTTON_FONTLIST);
    sw->toggle_switch.font = defaults ? XmFontListCopy(defaults) : NULL;
    if (sw->toggle_switch.font && sw->toggle_switch.label)
        XmStringExtent(sw->toggle_switch.font, sw->toggle_switch.label,
                       &label_width, &label_height);
    if (!sw->core.width)
        sw->core.width = label_width + 36 + sw->toggle_switch.spacing;
    if (!sw->core.height)
        sw->core.height = (label_height > 18 ? label_height : 18) + 8;

    sw->toggle_switch.track_color = sw->primitive.bottom_shadow_color;
    sw->toggle_switch.knob_color = sw->primitive.foreground;
    values.foreground = sw->toggle_switch.track_color;
    values.background = sw->core.background_pixel;
    sw->toggle_switch.track_gc = XCreateGC(display, DefaultRootWindow(display),
                                            mask, &values);
    values.foreground = sw->toggle_switch.knob_color;
    sw->toggle_switch.knob_gc = XCreateGC(display, DefaultRootWindow(display),
                                           mask, &values);
    values.foreground = sw->primitive.bottom_shadow_color;
    sw->toggle_switch.insensitive_gc = XCreateGC(
        display, DefaultRootWindow(display), mask, &values);
}

static void Destroy(Widget widget) {
    XmToggleSwitchWidget sw = (XmToggleSwitchWidget)widget;
    Display *display = XtDisplay(widget);
    if (sw->toggle_switch.track_gc) XFreeGC(display, sw->toggle_switch.track_gc);
    if (sw->toggle_switch.knob_gc) XFreeGC(display, sw->toggle_switch.knob_gc);
    if (sw->toggle_switch.insensitive_gc)
        XFreeGC(display, sw->toggle_switch.insensitive_gc);
    if (sw->toggle_switch.label) XmStringFree(sw->toggle_switch.label);
    if (sw->toggle_switch.font) XmFontListFree(sw->toggle_switch.font);
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmToggleSwitchWidget sw = (XmToggleSwitchWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    Dimension track_width = 28, track_height = 16;
    int track_x = (int)sw->toggle_switch.spacing;
    int track_y = ((int)sw->core.height - (int)track_height) / 2;
    int knob_x = sw->toggle_switch.set ? track_x + 14 : track_x + 2;
    int knob_y = track_y + 2;
    GC knob_gc;
    GC label_gc;
    Dimension label_width = 0, label_height = 0;
    int label_x, label_y;
    (void)event; (void)region;
    if (!window) return;
    update_gcs(sw);
    knob_gc = sw->toggle_switch.knob_gc ? sw->toggle_switch.knob_gc
                                        : sw->toggle_switch.track_gc;
    label_gc = XtIsSensitive(widget) ? knob_gc
                                     : (sw->toggle_switch.insensitive_gc ?
                                        sw->toggle_switch.insensitive_gc : knob_gc);
    if (!sw->toggle_switch.track_gc || !knob_gc || !label_gc) return;
    XClearWindow(display, window);
    XFillRectangle(display, window, sw->toggle_switch.track_gc,
                   track_x + 8, track_y, track_width - 16, track_height);
    XFillArc(display, window, sw->toggle_switch.track_gc,
             track_x, track_y, 16, track_height, 90 * 64, 180 * 64);
    XFillArc(display, window, sw->toggle_switch.track_gc,
             track_x + track_width - 16, track_y, 16, track_height,
             270 * 64, 180 * 64);
    XFillArc(display, window, label_gc, knob_x, knob_y, 12, 12, 0, 360 * 64);
    if (sw->toggle_switch.font)
        XmStringExtent(sw->toggle_switch.font, sw->toggle_switch.label,
                       &label_width, &label_height);
    label_x = track_x + (int)track_width + (int)sw->toggle_switch.spacing;
    label_y = ((int)sw->core.height - (int)label_height) / 2;
    if (sw->toggle_switch.font && sw->toggle_switch.label)
        XmStringDraw(display, window, sw->toggle_switch.font,
                     sw->toggle_switch.label, label_gc, label_x, label_y,
                     label_width, XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(sw), NULL);
    if (sw->primitive.highlighted && sw->primitive.highlight_thickness &&
        sw->core.width > 2 * sw->primitive.highlight_thickness &&
        sw->core.height > 2 * sw->primitive.highlight_thickness) {
        XDrawRectangle(display, window, knob_gc,
                       sw->primitive.highlight_thickness - 1,
                       sw->primitive.highlight_thickness - 1,
                       sw->core.width - 2 * sw->primitive.highlight_thickness,
                       sw->core.height - 2 * sw->primitive.highlight_thickness);
    }
}
static Boolean SetValues(Widget current, Widget request, Widget new_widget,
                         ArgList args, Cardinal *num_args) {
    XmToggleSwitchWidget old_sw = (XmToggleSwitchWidget)current;
    XmToggleSwitchWidget new_sw = (XmToggleSwitchWidget)new_widget;
    Boolean redraw = False;
    XmString replacement;
    (void)request; (void)args; (void)num_args;
    if (old_sw->toggle_switch.set != new_sw->toggle_switch.set) redraw = True;
    if (old_sw->toggle_switch.label != new_sw->toggle_switch.label) {
        replacement = new_sw->toggle_switch.label != NULL
            ? XmStringCopy(new_sw->toggle_switch.label) : NULL;
        if (new_sw->toggle_switch.label == NULL || replacement != NULL) {
            if (old_sw->toggle_switch.label)
                XmStringFree(old_sw->toggle_switch.label);
            new_sw->toggle_switch.label = replacement;
        } else {
            new_sw->toggle_switch.label = old_sw->toggle_switch.label;
        }
        redraw = True;
    }
    if (old_sw->primitive.foreground != new_sw->primitive.foreground ||
        old_sw->core.background_pixel != new_sw->core.background_pixel ||
        old_sw->primitive.highlighted != new_sw->primitive.highlighted)
        redraw = True;
    return redraw;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmToggleSwitchWidget sw = (XmToggleSwitchWidget)widget;
    Dimension width = 36 + sw->toggle_switch.spacing;
    Dimension height = 26;
    Dimension label_width = 0, label_height = 0;
    (void)request;
    if (sw->toggle_switch.font)
        XmStringExtent(sw->toggle_switch.font, sw->toggle_switch.label,
                       &label_width, &label_height);
    width += label_width;
    if (label_height + 8 > height) height = label_height + 8;
    reply->request_mode = CWWidth | CWHeight;
    reply->width = width; reply->height = height;
    return (widget->core.width == width && widget->core.height == height)
               ? XtGeometryYes : XtGeometryAlmost;
}

static void Toggle(Widget widget, XEvent *event, String *params,
                   Cardinal *num_params) {
    (void)params; (void)num_params;
    XmToggleSwitchSetState(widget,
                           !XmToggleSwitchGetState(widget), True);
    if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget),
                                        0, 0, 0, 0, True);
    (void)event;
}

Boolean XmToggleSwitchGetState(Widget widget) {
    if (!XmIsToggleSwitch(widget)) return False;
    return ((XmToggleSwitchWidget)widget)->toggle_switch.set;
}

void XmToggleSwitchSetState(Widget widget, Boolean state, Boolean notify) {
    XmToggleSwitchWidget sw;
    XmToggleSwitchCallbackStruct callback;
    Boolean changed;
    if (!XmIsToggleSwitch(widget)) return;
    sw = (XmToggleSwitchWidget)widget;
    state = state ? True : False;
    changed = sw->toggle_switch.set != state;
    sw->toggle_switch.set = state;
    if (XtIsRealized(widget))
        XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True);
    if (!notify || !changed || widget->core.being_destroyed) return;
    callback.reason = XmCR_VALUE_CHANGED;
    callback.event = NULL;
    callback.set = state;
    XtCallCallbacks(widget, XmNvalueChangedCallback, &callback);
}

Widget XmCreateToggleSwitch(Widget parent, char *name, ArgList args,
                            Cardinal arg_count) {
    return XtCreateWidget(name, xmToggleSwitchWidgetClass, parent, args,
                          arg_count);
}

Widget XmVaCreateToggleSwitch(Widget parent, char *name, ...) {
    Widget widget;
    va_list var;
    int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name);
    widget = XmeVLCreateWidget(name, xmToggleSwitchWidgetClass, parent, False,
                               var, count);
    va_end(var);
    return widget;
}

Widget XmVaCreateManagedToggleSwitch(Widget parent, char *name, ...) {
    Widget widget;
    va_list var;
    int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name);
    widget = XmeVLCreateWidget(name, xmToggleSwitchWidgetClass, parent, True,
                               var, count);
    va_end(var);
    return widget;
}
