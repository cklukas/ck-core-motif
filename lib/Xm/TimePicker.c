#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>
#include <Xm/TimePickerP.h>
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

#define Offset(field) XtOffsetOf(XmTimePickerRec, field)
static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmTimePickerWidget picker = (XmTimePickerWidget)widget;
    (void)offset;
    if (picker->time_picker.check_set_render_table)
        value->addr = NULL;
    else {
        picker->time_picker.check_set_render_table = True;
        value->addr = (char *)&picker->time_picker.render_table;
    }
}
static XtResource resources[] = {
    {XmNtimePickerHour, XmCTimePickerHour, XmRInt, sizeof(int),
     Offset(time_picker.value.hour), XmRImmediate, (XtPointer)0},
    {XmNtimePickerMinute, XmCTimePickerMinute, XmRInt, sizeof(int),
     Offset(time_picker.value.minute), XmRImmediate, (XtPointer)0},
    {XmNtimePickerSecond, XmCTimePickerSecond, XmRInt, sizeof(int),
     Offset(time_picker.value.second), XmRImmediate, (XtPointer)0},
    {XmNtimePickerFormat24Hour, XmCTimePickerFormat24Hour, XmRBoolean,
     sizeof(Boolean), Offset(time_picker.format_24_hour), XmRImmediate,
     (XtPointer)True},
    {XmNtimePickerValueChangedCallback, XmCTimePickerValueChangedCallback,
     XmRCallback, sizeof(XtCallbackList),
     Offset(time_picker.value_changed_callback), XmRPointer, (XtPointer)NULL},
    {XmNtimePickerActivateCallback, XmCTimePickerActivateCallback,
     XmRCallback, sizeof(XtCallbackList),
     Offset(time_picker.activate_callback), XmRPointer, (XtPointer)NULL},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
     Offset(time_picker.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmFontList),
     Offset(time_picker.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};

externaldef(xmtimepickerclassrec) XmTimePickerClassRec xmTimePickerClassRec =
    {{(WidgetClass)&xmPrimitiveClassRec, "XmTimePicker", sizeof(XmTimePickerRec),
      NULL, NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0,
      resources, XtNumber(resources), NULLQUARK, True,
      XtExposeCompressMaximal, True, False, Destroy, NULL, Redisplay,
      SetValues, NULL, XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
      XtInheritTranslations, QueryGeometry, NULL, NULL},
     {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
      XtInheritTranslations, NULL, NULL, 0, NULL},
     {NULL}};

externaldef(xmtimepickerwidgetclass) WidgetClass xmTimePickerWidgetClass =
    (WidgetClass)&xmTimePickerClassRec;

Boolean XmTimePickerIsValid(const XmTimePickerValue *value) {
    return value && value->hour >= 0 && value->hour <= 23 &&
           value->minute >= 0 && value->minute <= 59 &&
           value->second >= 0 && value->second <= 59;
}

static int compare_values(const XmTimePickerValue *a,
                          const XmTimePickerValue *b) {
    if (a->hour != b->hour) return a->hour < b->hour ? -1 : 1;
    if (a->minute != b->minute) return a->minute < b->minute ? -1 : 1;
    if (a->second != b->second) return a->second < b->second ? -1 : 1;
    return 0;
}

static int to_seconds(const XmTimePickerValue *value) {
    return value->hour * 3600 + value->minute * 60 + value->second;
}

static XmTimePickerValue from_seconds(int seconds) {
    XmTimePickerValue value;
    value.hour = seconds / 3600;
    value.minute = (seconds / 60) % 60;
    value.second = seconds % 60;
    return value;
}

static void notify(XmTimePickerWidget picker, XEvent *event, String name,
                   int reason) {
    XmTimePickerCallbackStruct callback;
    XtCallbackList list = name &&
        strcmp(name, XmNtimePickerActivateCallback) == 0
        ? picker->time_picker.activate_callback
        : picker->time_picker.value_changed_callback;
    if (!list) return;
    callback.reason = reason;
    callback.event = event;
    callback.value = picker->time_picker.value;
    XtCallCallbacks((Widget)picker, name, &callback);
}

static Boolean set_value_internal(XmTimePickerWidget picker,
                                  const XmTimePickerValue *value,
                                  Boolean notify_change, XEvent *event) {
    if (!XmTimePickerIsValid(value) ||
        compare_values(value, &picker->time_picker.minimum) < 0 ||
        compare_values(value, &picker->time_picker.maximum) > 0 ||
        compare_values(value, &picker->time_picker.value) == 0)
        return False;
    picker->time_picker.value = *value;
    if (XtIsRealized((Widget)picker))
        XClearArea(XtDisplay((Widget)picker), XtWindow((Widget)picker),
                   0, 0, 0, 0, True);
    if (notify_change)
        notify(picker, event, XmNtimePickerValueChangedCallback, 1);
    return True;
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmTimePickerWidget picker = (XmTimePickerWidget)widget;
    XGCValues values;
    (void)request; (void)args; (void)count;
    if (!XmTimePickerIsValid(&picker->time_picker.value))
        picker->time_picker.value = (XmTimePickerValue){0, 0, 0};
    picker->time_picker.minimum = (XmTimePickerValue){0, 0, 0};
    picker->time_picker.maximum = (XmTimePickerValue){23, 59, 59};
    if (!picker->time_picker.render_table)
        picker->time_picker.render_table = XmFontListCopy(
            XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
    else
        picker->time_picker.render_table = XmFontListCopy(
            picker->time_picker.render_table);
    picker->time_picker.field = 0;
    values.foreground = picker->primitive.foreground;
    values.background = picker->core.background_pixel;
    picker->time_picker.gc = XCreateGC(XtDisplay(widget),
                                       DefaultRootWindow(XtDisplay(widget)),
                                       GCForeground | GCBackground, &values);
    if (!picker->core.width) picker->core.width = 96;
    if (!picker->core.height) picker->core.height = 28;
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events,
                      NULL);
}

static void Destroy(Widget widget) {
    XmTimePickerWidget picker = (XmTimePickerWidget)widget;
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask, False,
                         Events, NULL);
    if (picker->time_picker.gc)
        XFreeGC(XtDisplay(widget), picker->time_picker.gc);
    if (picker->time_picker.render_table)
        XmRenderTableFree(picker->time_picker.render_table);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget new_widget,
                         ArgList args, Cardinal *count) {
    XmTimePickerWidget old = (XmTimePickerWidget)old_widget;
    XmTimePickerWidget picker = (XmTimePickerWidget)new_widget;
    Boolean render_changed = old->time_picker.render_table != picker->time_picker.render_table;
    (void)request; (void)args; (void)count;
    if (!XmTimePickerIsValid(&picker->time_picker.value) ||
        compare_values(&picker->time_picker.value, &old->time_picker.minimum) < 0 ||
        compare_values(&picker->time_picker.value, &old->time_picker.maximum) > 0)
        picker->time_picker.value = old->time_picker.value;
    if (render_changed) {
        XmFontList replacement = picker->time_picker.render_table
            ? XmFontListCopy(picker->time_picker.render_table)
            : XmFontListCopy(XmeGetDefaultRenderTable(new_widget, XmLABEL_FONTLIST));
        if (old->time_picker.render_table)
            XmRenderTableFree(old->time_picker.render_table);
        picker->time_picker.render_table = replacement;
    }
    return compare_values(&old->time_picker.value, &picker->time_picker.value) != 0 ||
           old->time_picker.format_24_hour != picker->time_picker.format_24_hour || render_changed;
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmTimePickerWidget picker = (XmTimePickerWidget)widget;
    char text[32];
    XmString value;
    Dimension width = 0, height = 0;
    int hour = picker->time_picker.value.hour;
    (void)event; (void)region;
    if (!XtIsRealized(widget) || !picker->time_picker.gc) return;
    XClearWindow(XtDisplay(widget), XtWindow(widget));
    if (picker->time_picker.format_24_hour)
        snprintf(text, sizeof(text), "%02d:%02d:%02d", hour,
                 picker->time_picker.value.minute,
                 picker->time_picker.value.second);
    else {
        int display_hour = hour % 12;
        if (!display_hour) display_hour = 12;
        snprintf(text, sizeof(text), "%02d:%02d:%02d %s", display_hour,
                 picker->time_picker.value.minute,
                 picker->time_picker.value.second, hour < 12 ? "AM" : "PM");
    }
    value = XmStringCreateLocalized(text);
    if (value && picker->time_picker.render_table) {
        XmStringExtent(picker->time_picker.render_table, value, &width, &height);
        XmStringDraw(XtDisplay(widget), XtWindow(widget),
                     picker->time_picker.render_table, value,
                     picker->time_picker.gc, 6,
                     ((int)picker->core.height - (int)height) / 2, width,
                     XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(picker), NULL);
    }
    XmStringFree(value);
}

static void Events(Widget widget, XtPointer closure, XEvent *event,
                   Boolean *continue_dispatch) {
    XmTimePickerWidget picker = (XmTimePickerWidget)widget;
    KeySym key;
    char input[2];
    XmTimePickerValue value;
    int seconds, delta;
    (void)closure; (void)continue_dispatch;
    if (event->type == ButtonPress) {
        picker->time_picker.field = (unsigned char)((event->xbutton.x < 34) ? 0 :
                                      (event->xbutton.x < 66 ? 1 : 2));
        return;
    }
    if (event->type != KeyPress ||
        XLookupString(&event->xkey, input, sizeof(input), &key, NULL) == 0)
        return;
    if (key == XK_Left || key == XK_Right) {
        if (key == XK_Left && picker->time_picker.field) --picker->time_picker.field;
        if (key == XK_Right && picker->time_picker.field < 2) ++picker->time_picker.field;
        return;
    }
    if (key == XK_Return || key == XK_KP_Enter) {
        notify(picker, event, XmNtimePickerActivateCallback, 2);
        return;
    }
    if (key != XK_Up && key != XK_Down) return;
    value = picker->time_picker.value;
    seconds = to_seconds(&value);
    delta = key == XK_Up ? 1 : -1;
    if (picker->time_picker.field == 0) delta *= 3600;
    else if (picker->time_picker.field == 1) delta *= 60;
    seconds += delta;
    if (seconds < 0) seconds = 0;
    if (seconds > 86399) seconds = 86399;
    value = from_seconds(seconds);
    set_value_internal(picker, &value, True, event);
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    (void)widget; (void)request;
    reply->request_mode = CWWidth | CWHeight;
    reply->width = 96; reply->height = 28;
    return XtGeometryAlmost;
}

Widget XmCreateTimePicker(Widget parent, char *name, ArgList args,
                          Cardinal count) {
    return XtCreateWidget(name, xmTimePickerWidgetClass, parent, args, count);
}

Boolean XmTimePickerGetValue(Widget widget, XmTimePickerValue *value_return) {
    if (!widget || !value_return || !XmIsTimePicker(widget)) return False;
    *value_return = ((XmTimePickerWidget)widget)->time_picker.value;
    return True;
}

Boolean XmTimePickerSetValue(Widget widget, const XmTimePickerValue *value,
                             Boolean notify_change) {
    if (!widget || !XmIsTimePicker(widget)) return False;
    return set_value_internal((XmTimePickerWidget)widget, value, notify_change,
                              NULL);
}

Boolean XmTimePickerGetRange(Widget widget, XmTimePickerValue *minimum_return,
                             XmTimePickerValue *maximum_return) {
    XmTimePickerWidget picker;
    if (!widget || !minimum_return || !maximum_return || !XmIsTimePicker(widget))
        return False;
    picker = (XmTimePickerWidget)widget;
    *minimum_return = picker->time_picker.minimum;
    *maximum_return = picker->time_picker.maximum;
    return True;
}

Boolean XmTimePickerSetRange(Widget widget, const XmTimePickerValue *minimum,
                             const XmTimePickerValue *maximum) {
    XmTimePickerWidget picker;
    if (!widget || !XmIsTimePicker(widget) || !XmTimePickerIsValid(minimum) ||
        !XmTimePickerIsValid(maximum) || compare_values(minimum, maximum) > 0)
        return False;
    picker = (XmTimePickerWidget)widget;
    if (compare_values(&picker->time_picker.value, minimum) < 0 ||
        compare_values(&picker->time_picker.value, maximum) > 0)
        return False;
    picker->time_picker.minimum = *minimum;
    picker->time_picker.maximum = *maximum;
    return True;
}
