/* Additive bounded Xt-timer spinner control.  No worker thread is used. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/SpinnerP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "XmI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);
static void Events(Widget, XtPointer, XEvent *, Boolean *);
static void Tick(XtPointer, XtIntervalId *);

#define Offset(field) XtOffsetOf(XmSpinnerRec, field)
static XtResource resources[] = {
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
        XtExposeCompressMaximal, True, False, Destroy, NULL, Redisplay,
        SetValues, NULL, XtInheritSetValuesAlmost, NULL, NULL, XtVersion,
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
    Dimension label_width = 0, label_height = 0;
    unsigned long mask = GCForeground | GCBackground;
    (void)request; (void)args; (void)count;
    spinner->spinner.interval = bounded_interval(spinner->spinner.interval);
    spinner->spinner.label = spinner->spinner.label
        ? XmStringCopy(spinner->spinner.label)
        : XmStringCreateLocalized("");
    defaults = XmeGetDefaultRenderTable(widget, XmTEXT_FONTLIST);
    spinner->spinner.font = defaults ? XmFontListCopy(defaults) : NULL;
    if (spinner->spinner.font)
        XmStringExtent(spinner->spinner.font, spinner->spinner.label,
                       &label_width, &label_height);
    if (!spinner->core.width)
        spinner->core.width = label_width + 40 + 2 * spinner->spinner.spacing;
    if (!spinner->core.height)
        spinner->core.height = (label_height > 28 ? label_height : 28) +
                               2 * spinner->spinner.spacing;
    values.foreground = spinner->primitive.foreground;
    values.background = spinner->core.background_pixel;
    spinner->spinner.ring_gc = XCreateGC(display, DefaultRootWindow(display),
                                         mask, &values);
    spinner->spinner.text_gc = XCreateGC(display, DefaultRootWindow(display),
                                         mask, &values);
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
    if (spinner->spinner.ring_gc)
        XSetForeground(display, spinner->spinner.ring_gc,
                       spinner->primitive.foreground);
    if (spinner->spinner.text_gc)
        XSetForeground(display, spinner->spinner.text_gc,
                       spinner->primitive.foreground);
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmSpinnerWidget spinner = (XmSpinnerWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    Dimension label_width = 0, label_height = 0;
    Dimension diameter;
    Position center_x, center_y;
    GC gc;
    int start;
    (void)event; (void)region;
    if (!window || !spinner->core.width || !spinner->core.height) return;
    UpdateGCs(spinner);
    gc = spinner->spinner.ring_gc ? spinner->spinner.ring_gc
                                  : spinner->spinner.text_gc;
    if (!gc) return;
    XClearWindow(display, window);
    if (spinner->spinner.font)
        XmStringExtent(spinner->spinner.font, spinner->spinner.label,
                       &label_width, &label_height);
    {
        int available = (int)spinner->core.height -
                        2 * (int)spinner->spinner.spacing;
        diameter = available > 16 ? (Dimension)available : 16;
    }
    if (diameter > spinner->core.width) diameter = spinner->core.width;
    if (diameter > 32) diameter = 32;
    center_x = (Position)(spinner->spinner.spacing + diameter / 2);
    center_y = (Position)(spinner->core.height / 2);
    start = (int)((spinner->spinner.phase % 12) * 30) * 64;
    XDrawArc(display, window, gc, center_x - (Position)diameter / 2,
             center_y - (Position)diameter / 2, diameter, diameter,
             start, 250 * 64);
    if (spinner->spinner.font && spinner->spinner.label) {
        XmStringDraw(display, window, spinner->spinner.font,
                     spinner->spinner.label, gc,
                     (Position)(spinner->spinner.spacing + diameter + 8),
                     (Position)((spinner->core.height - label_height) / 2),
                     label_width, XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(spinner), NULL);
    }
    if (spinner->primitive.highlighted && spinner->primitive.highlight_thickness &&
        spinner->core.width > 2 * spinner->primitive.highlight_thickness &&
        spinner->core.height > 2 * spinner->primitive.highlight_thickness)
        XDrawRectangle(display, window, gc,
                       spinner->primitive.highlight_thickness - 1,
                       spinner->primitive.highlight_thickness - 1,
                       spinner->core.width - 2 * spinner->primitive.highlight_thickness,
                       spinner->core.height - 2 * spinner->primitive.highlight_thickness);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmSpinnerWidget old = (XmSpinnerWidget)old_widget;
    XmSpinnerWidget spinner = (XmSpinnerWidget)widget;
    Boolean redraw = False;
    (void)request; (void)args; (void)count;
    spinner->spinner.interval = bounded_interval(spinner->spinner.interval);
    if (old->spinner.label != spinner->spinner.label) {
        if (spinner->spinner.label)
            spinner->spinner.label = XmStringCopy(spinner->spinner.label);
        if (old->spinner.label) XmStringFree(old->spinner.label);
        redraw = True;
    }
    if (old->spinner.active != spinner->spinner.active) {
        redraw = True;
        SyncTimer(spinner);
    }
    if (old->spinner.interval != spinner->spinner.interval ||
        old->primitive.foreground != spinner->primitive.foreground ||
        old->core.background_pixel != spinner->core.background_pixel ||
        old->primitive.highlighted != spinner->primitive.highlighted)
        redraw = True;
    return redraw;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmSpinnerWidget spinner = (XmSpinnerWidget)widget;
    Dimension width = 0, height = 0;
    (void)request;
    if (spinner->spinner.font)
        XmStringExtent(spinner->spinner.font, spinner->spinner.label,
                       &width, &height);
    reply->request_mode = CWWidth | CWHeight;
    reply->width = width + 40 + 2 * spinner->spinner.spacing;
    reply->height = (height > 28 ? height : 28) + 2 * spinner->spinner.spacing;
    return (widget->core.width == reply->width && widget->core.height == reply->height)
               ? XtGeometryYes : XtGeometryAlmost;
}

static void Events(Widget widget, XtPointer data, XEvent *event,
                   Boolean *dispatch) {
    (void)data; (void)dispatch;
    if (event->type == MapNotify || event->type == UnmapNotify ||
        event->type == VisibilityNotify)
        SyncTimer((XmSpinnerWidget)widget);
}

static void Tick(XtPointer data, XtIntervalId *id) {
    XmSpinnerWidget spinner = (XmSpinnerWidget)data;
    (void)id;
    spinner->spinner.timer_pending = False;
    if (!eligible(spinner)) return;
    spinner->spinner.phase = (spinner->spinner.phase + 1) % 12;
    if (XtIsRealized((Widget)spinner))
        XClearArea(XtDisplay((Widget)spinner), XtWindow((Widget)spinner),
                   0, 0, 0, 0, True);
    SyncTimer(spinner);
}

static void Notify(XmSpinnerWidget spinner, XEvent *event) {
    XmSpinnerCallbackStruct callback;
    callback.reason = XmCR_VALUE_CHANGED;
    callback.event = event;
    callback.active = spinner->spinner.active;
    callback.phase = spinner->spinner.phase;
    XtCallCallbacks((Widget)spinner, XmNspinnerStateChangedCallback,
                    &callback);
}

Boolean XmSpinnerGetActive(Widget widget) {
    return XmIsSpinner(widget) ? ((XmSpinnerWidget)widget)->spinner.active : False;
}
void XmSpinnerSetActive(Widget widget, Boolean active, Boolean notify) {
    XmSpinnerWidget spinner;
    Boolean old;
    if (!XmIsSpinner(widget)) return;
    spinner = (XmSpinnerWidget)widget;
    old = spinner->spinner.active;
    spinner->spinner.active = active ? True : False;
    SyncTimer(spinner);
    if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget),
                                         0, 0, 0, 0, True);
    if (notify && old != spinner->spinner.active)
        Notify(spinner, NULL);
}
unsigned int XmSpinnerGetPhase(Widget widget) {
    return XmIsSpinner(widget) ? ((XmSpinnerWidget)widget)->spinner.phase : 0;
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
