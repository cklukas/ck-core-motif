#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "PaintI.h"
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <Xm/TimePickerP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "XmI.h"
#include "AppearanceDrawI.h"

static Position bounded_position(long long value) {
    if (value < SHRT_MIN) return (Position)SHRT_MIN;
    if (value > SHRT_MAX) return (Position)SHRT_MAX;
    return (Position)value;
}

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void BufferedRedisplay(Widget, XEvent *, Region);
static Boolean BufferedSetValues(Widget, Widget, Widget, ArgList, Cardinal *);
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
      XtExposeCompressMaximal, True, False, Destroy, NULL, BufferedRedisplay,
      BufferedSetValues, NULL, XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
      (String)_XmPrimitiveTabFocusTranslations, QueryGeometry, NULL, NULL},
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
    if (picker == NULL || picker->core.being_destroyed)
        return;
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
        _XmPaintInvalidate((Widget)picker);
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
    {Dimension width=96,height=28;
     (void)_XmAppearancePickerSize(widget,picker->time_picker.render_table,picker->time_picker.format_24_hour?"00:00:00":"00:00:00 AM",&width,&height);
     if(!request->core.width)picker->core.width=width;
     if(!request->core.height)picker->core.height=height;}
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
    Boolean direction_changed = old->primitive.layout_direction !=
                                picker->primitive.layout_direction;
    Boolean colors_changed = old->primitive.foreground != picker->primitive.foreground ||
                             old->core.background_pixel != picker->core.background_pixel;
    (void)request; (void)args; (void)count;
    if (colors_changed && picker->time_picker.gc) {
        XSetForeground(XtDisplay(new_widget), picker->time_picker.gc,
                       picker->primitive.foreground);
        XSetBackground(XtDisplay(new_widget), picker->time_picker.gc,
                       picker->core.background_pixel);
    }
    if (!XmTimePickerIsValid(&picker->time_picker.value) ||
        compare_values(&picker->time_picker.value, &old->time_picker.minimum) < 0 ||
        compare_values(&picker->time_picker.value, &old->time_picker.maximum) > 0)
        picker->time_picker.value = old->time_picker.value;
    if (render_changed) {
        XmFontList replacement = picker->time_picker.render_table
            ? XmFontListCopy(picker->time_picker.render_table)
            : XmFontListCopy(XmeGetDefaultRenderTable(new_widget, XmLABEL_FONTLIST));
        if (replacement) {
            if (old->time_picker.render_table)
                XmRenderTableFree(old->time_picker.render_table);
            picker->time_picker.render_table = replacement;
        } else {
            picker->time_picker.render_table = old->time_picker.render_table;
            render_changed = False;
        }
    }
    return colors_changed || compare_values(&old->time_picker.value, &picker->time_picker.value) != 0 ||
           old->time_picker.format_24_hour != picker->time_picker.format_24_hour || render_changed ||
           direction_changed;
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmTimePickerWidget picker = (XmTimePickerWidget)widget;
    char text[32];
    XmString value;
    Dimension width = 0, height = 0;
    int hour = picker->time_picker.value.hour;
    (void)event; (void)region;
    if (!XtIsRealized(widget) || !picker->time_picker.gc) return;
    Boolean modern=_XmAppearanceDrawPickerFrame(widget,True);
    XSetForeground(XtDisplay(widget),picker->time_picker.gc,XtIsSensitive(widget)?picker->primitive.foreground:picker->primitive.bottom_shadow_color);

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
        XmStringDraw(XtDisplay(widget), _XmPaintDrawable(widget),
                     picker->time_picker.render_table, value,
                     picker->time_picker.gc, bounded_position(modern?_XmDpiDefaultDimension(widget,12):6),
                     bounded_position(((long long)picker->core.height - height) / 2), width,
                     XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(picker), NULL);
    }
    if(modern&&picker->primitive.highlighted&&value) {
        const char *prefix=picker->time_picker.field==0?"":picker->time_picker.field==1?"00:":"00:00:";
        XmString before=XmStringCreateLocalized((char*)prefix),digits=XmStringCreateLocalized("00");Dimension pw=0,ph=0,dw=0,dh=0;
        XmStringExtent(picker->time_picker.render_table,before,&pw,&ph);XmStringExtent(picker->time_picker.render_table,digits,&dw,&dh);XmStringFree(before);XmStringFree(digits);
        int y=((int)picker->core.height+height)/2+_XmDpiDefaultDimension(widget,2);
        XSetForeground(XtDisplay(widget),picker->time_picker.gc,picker->primitive.highlight_color);
        XFillRectangle(XtDisplay(widget),_XmPaintDrawable(widget),picker->time_picker.gc,_XmDpiDefaultDimension(widget,12)+pw,y,dw,MAX(1,_XmDpiDefaultDimension(widget,1)));
    }
    XmStringFree(value);
}

static void BufferedRedisplay(Widget widget, XEvent *event, Region region)
{
    _XmBufferedPaint(widget, Redisplay, event, region);
}

static void Events(Widget widget, XtPointer closure, XEvent *event,
                   Boolean *continue_dispatch) {
    XmTimePickerWidget picker = (XmTimePickerWidget)widget;
    KeySym key;
    char input[2];
    XmTimePickerValue value;
    int seconds, delta;
    (void)closure; (void)continue_dispatch;
    if (picker == NULL || event == NULL || !XmIsTimePicker(widget))
	return;
    if (!_XmPrimitiveFocusOnPrimaryClick(widget, event)) return;
    if (event->type == ButtonPress) {
        XmAppearanceProfile profile=_XmAppearanceGeometryProfile(XtDisplay(widget));
        Boolean modern=profile==XmAPPEARANCE_MODERN_LIGHT||profile==XmAPPEARANCE_MODERN_DARK;
        if(modern&&event->xbutton.x>=(int)picker->core.width-(int)_XmDpiDefaultDimension(widget,36)) {
            key=event->xbutton.y<(int)picker->core.height/2?XK_Up:XK_Down;
        } else {
            int left=34,right=66;
            if(modern){XmString first=XmStringCreateLocalized("00:"),second=XmStringCreateLocalized("00:00:");Dimension w=0,h=0;XmStringExtent(picker->time_picker.render_table,first,&w,&h);left=_XmDpiDefaultDimension(widget,12)+w;XmStringExtent(picker->time_picker.render_table,second,&w,&h);right=_XmDpiDefaultDimension(widget,12)+w;XmStringFree(first);XmStringFree(second);}
            picker->time_picker.field=(unsigned char)(event->xbutton.x<left?0:event->xbutton.x<right?1:2);
            _XmPaintInvalidate(widget);return;
        }
    } else {
        if (event->type != KeyPress) return;
        XLookupString(&event->xkey,input,sizeof(input),&key,NULL);
    }
    if (continue_dispatch && (key == XK_Left || key == XK_Right || key == XK_Up ||
        key == XK_Down || key == XK_Return || key == XK_KP_Enter))
        *continue_dispatch = False;
    if (key == XK_Left || key == XK_Right) {
        Boolean rtl = XmDirectionMatchPartial(XmPrim_layout_direction(picker),
                                              XmRIGHT_TO_LEFT,
                                              XmHORIZONTAL_MASK);
        if (key == XK_Left) {
            if (rtl && picker->time_picker.field < 2) ++picker->time_picker.field;
            else if (!rtl && picker->time_picker.field) --picker->time_picker.field;
        } else {
            if (rtl && picker->time_picker.field) --picker->time_picker.field;
            else if (!rtl && picker->time_picker.field < 2) ++picker->time_picker.field;
        }
        _XmPaintInvalidate(widget);return;
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
    XmTimePickerWidget picker=(XmTimePickerWidget)widget;
    reply->request_mode=CWWidth|CWHeight;
    reply->width=96;reply->height=28;
    (void)_XmAppearancePickerSize(widget,picker->time_picker.render_table,picker->time_picker.format_24_hour?"00:00:00":"00:00:00 AM",&reply->width,&reply->height);
    return XmeReplyToQueryGeometry(widget,request,reply);
}

Widget XmCreateTimePicker(Widget parent, char *name, ArgList args,
                          Cardinal count) {
    return XtCreateWidget(name, xmTimePickerWidgetClass, parent, args, count);
}

Boolean XmTimePickerGetValue(Widget widget, XmTimePickerValue *value_return) {
    if (!widget || !value_return || !XmIsTimePicker(widget) ||
        widget->core.being_destroyed) return False;
    *value_return = ((XmTimePickerWidget)widget)->time_picker.value;
    return True;
}

Boolean XmTimePickerSetValue(Widget widget, const XmTimePickerValue *value,
                             Boolean notify_change) {
    if (!widget || !XmIsTimePicker(widget) || widget->core.being_destroyed)
        return False;
    return set_value_internal((XmTimePickerWidget)widget, value, notify_change,
                              NULL);
}

Boolean XmTimePickerGetRange(Widget widget, XmTimePickerValue *minimum_return,
                             XmTimePickerValue *maximum_return) {
    XmTimePickerWidget picker;
    if (!widget || !minimum_return || !maximum_return || !XmIsTimePicker(widget) ||
        widget->core.being_destroyed)
        return False;
    picker = (XmTimePickerWidget)widget;
    *minimum_return = picker->time_picker.minimum;
    *maximum_return = picker->time_picker.maximum;
    return True;
}

Boolean XmTimePickerSetRange(Widget widget, const XmTimePickerValue *minimum,
                             const XmTimePickerValue *maximum) {
    XmTimePickerWidget picker;
    if (!widget || !XmIsTimePicker(widget) || widget->core.being_destroyed ||
        !XmTimePickerIsValid(minimum) ||
        !XmTimePickerIsValid(maximum) || compare_values(minimum, maximum) > 0)
        return False;
    picker = (XmTimePickerWidget)widget;
    if (compare_values(&picker->time_picker.value, minimum) < 0 ||
        compare_values(&picker->time_picker.value, maximum) > 0)
        return False;
    picker->time_picker.minimum = *minimum;
    picker->time_picker.maximum = *maximum;
    if (XtIsRealized(widget))
        _XmPaintInvalidate(widget);
    return True;
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
