#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>
#include <Xm/DatePickerP.h>
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
static void calendar_changed(Widget, XtPointer, XtPointer);

#define Offset(field) XtOffsetOf(XmDatePickerRec, field)
static XtResource resources[] = {
    {XmNdatePickerYear, XmCDatePickerYear, XmRInt, sizeof(int),
     Offset(date_picker.date.year), XmRImmediate, (XtPointer)1970},
    {XmNdatePickerMonth, XmCDatePickerMonth, XmRInt, sizeof(int),
     Offset(date_picker.date.month), XmRImmediate, (XtPointer)1},
    {XmNdatePickerDay, XmCDatePickerDay, XmRInt, sizeof(int),
     Offset(date_picker.date.day), XmRImmediate, (XtPointer)1},
    {XmNdatePickerDateChangedCallback, XmCDatePickerDateChangedCallback,
     XmRCallback, sizeof(XtCallbackList),
     Offset(date_picker.date_changed_callback), XmRPointer, (XtPointer)NULL},
    {XmNdatePickerOpenChangedCallback, XmCDatePickerOpenChangedCallback,
     XmRCallback, sizeof(XtCallbackList),
     Offset(date_picker.open_changed_callback), XmRPointer, (XtPointer)NULL},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};

externaldef(xmdatepickerclassrec) XmDatePickerClassRec xmDatePickerClassRec =
    {{(WidgetClass)&xmPrimitiveClassRec, "XmDatePicker", sizeof(XmDatePickerRec),
      NULL, NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0,
      resources, XtNumber(resources), NULLQUARK, True,
      XtExposeCompressMaximal, True, False, Destroy, NULL, Redisplay,
      SetValues, NULL, XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
      XtInheritTranslations, QueryGeometry, NULL, NULL},
     {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
      XtInheritTranslations, NULL, NULL, 0, NULL},
     {NULL}};

externaldef(xmdatepickerwidgetclass) WidgetClass xmDatePickerWidgetClass =
    (WidgetClass)&xmDatePickerClassRec;

static int compare_dates(const XmDatePickerDate *a,
                         const XmDatePickerDate *b) {
    if (a->year != b->year) return a->year < b->year ? -1 : 1;
    if (a->month != b->month) return a->month < b->month ? -1 : 1;
    if (a->day != b->day) return a->day < b->day ? -1 : 1;
    return 0;
}

static void notify(XmDatePickerWidget picker, XEvent *event, int reason,
                   String name) {
    XmDatePickerCallbackStruct callback;
    XtCallbackList list = name &&
        strcmp(name, XmNdatePickerOpenChangedCallback) == 0
        ? picker->date_picker.open_changed_callback
        : picker->date_picker.date_changed_callback;
    if (!list)
        return;
    callback.reason = reason;
    callback.event = event;
    callback.date = picker->date_picker.date;
    XtCallCallbacks((Widget)picker, name, &callback);
}

static Widget shell_ancestor(Widget widget) {
    while (widget && !XtIsShell(widget))
        widget = XtParent(widget);
    return widget;
}

static void create_popup(XmDatePickerWidget picker) {
    Widget parent_shell = shell_ancestor(XtParent((Widget)picker));
    Arg args[4];
    Cardinal count = 0;
    if (picker->date_picker.popup_shell || !parent_shell)
        return;
    XtSetArg(args[count], XmNoverrideRedirect, True); ++count;
    XtSetArg(args[count], XmNmappedWhenManaged, False); ++count;
    XtSetArg(args[count], XmNsaveUnder, True); ++count;
    XtSetArg(args[count], XmNtransientFor, XtWindow(parent_shell)); ++count;
    picker->date_picker.popup_shell = XtCreatePopupShell(
        "datePickerShell", overrideShellWidgetClass, parent_shell, args, count);
    picker->date_picker.calendar = XmCreateCalendar(
        picker->date_picker.popup_shell, (char *)"calendar", NULL, 0);
    XtVaSetValues(picker->date_picker.calendar, XmNwidth, 224,
                  XmNheight, 192, NULL);
    XmCalendarSetRange(picker->date_picker.calendar,
                       &picker->date_picker.minimum,
                       &picker->date_picker.maximum);
    XmCalendarSetDate(picker->date_picker.calendar,
                      &picker->date_picker.date, False);
    XtAddCallback(picker->date_picker.calendar,
                  XmNcalendarDateChangedCallback, calendar_changed,
                  (XtPointer)picker);
    XtManageChild(picker->date_picker.calendar);
    XtRealizeWidget(picker->date_picker.popup_shell);
}

static void position_popup(XmDatePickerWidget picker) {
    Position x, y;
    Dimension width, height, popup_width, popup_height;
    Screen *screen;
    Widget owner = (Widget)picker;
    if (!picker->date_picker.popup_shell || !XtIsRealized(owner))
        return;
    XtTranslateCoords(owner, 0, 0, &x, &y);
    XtVaGetValues(owner, XmNwidth, &width, XmNheight, &height, NULL);
    XtVaGetValues(picker->date_picker.popup_shell, XmNwidth, &popup_width,
                  XmNheight, &popup_height, NULL);
    x += (Position)((int)width - (int)popup_width) / 2;
    y += (Position)height;
    screen = XtScreen(picker->date_picker.popup_shell);
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if ((int)x + (int)popup_width > WidthOfScreen(screen))
        x = (Position)(WidthOfScreen(screen) - popup_width);
    if ((int)y + (int)popup_height > HeightOfScreen(screen))
        y = (Position)(HeightOfScreen(screen) - popup_height);
    XtVaSetValues(picker->date_picker.popup_shell, XtNx, x, XtNy, y, NULL);
}

static void calendar_changed(Widget calendar, XtPointer closure,
                             XtPointer call_data) {
    XmDatePickerWidget picker = (XmDatePickerWidget)closure;
    XmDatePickerDate date;
    (void)call_data;
    if (!XmCalendarGetDate(calendar, &date))
        return;
    picker->date_picker.date = date;
    if (picker->date_picker.open) {
        XtPopdown(picker->date_picker.popup_shell);
        picker->date_picker.open = False;
    }
    notify(picker, NULL, 1, XmNdatePickerDateChangedCallback);
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmDatePickerWidget picker = (XmDatePickerWidget)widget;
    XGCValues values;
    (void)request; (void)args; (void)count;
    if (!XmCalendarIsValidDate(&picker->date_picker.date))
        picker->date_picker.date = (XmDatePickerDate){1970, 1, 1};
    picker->date_picker.minimum = (XmDatePickerDate){1, 1, 1};
    picker->date_picker.maximum = (XmDatePickerDate){9999, 12, 31};
    values.foreground = picker->primitive.foreground;
    values.background = picker->core.background_pixel;
    picker->date_picker.gc = XCreateGC(XtDisplay(widget),
                                       DefaultRootWindow(XtDisplay(widget)),
                                       GCForeground | GCBackground, &values);
    if (!picker->core.width) picker->core.width = 120;
    if (!picker->core.height) picker->core.height = 28;
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events,
                      NULL);
}

static void Destroy(Widget widget) {
    XmDatePickerWidget picker = (XmDatePickerWidget)widget;
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask, False,
                         Events, NULL);
    if (picker->date_picker.popup_shell)
        XtDestroyWidget(picker->date_picker.popup_shell);
    if (picker->date_picker.gc)
        XFreeGC(XtDisplay(widget), picker->date_picker.gc);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget new_widget,
                         ArgList args, Cardinal *count) {
    XmDatePickerWidget old = (XmDatePickerWidget)old_widget;
    XmDatePickerWidget picker = (XmDatePickerWidget)new_widget;
    (void)request; (void)args; (void)count;
    if (!XmCalendarIsValidDate(&picker->date_picker.date) ||
        compare_dates(&picker->date_picker.date, &old->date_picker.minimum) < 0 ||
        compare_dates(&picker->date_picker.date, &old->date_picker.maximum) > 0)
        picker->date_picker.date = old->date_picker.date;
    return old->date_picker.date.year != picker->date_picker.date.year ||
           old->date_picker.date.month != picker->date_picker.date.month ||
           old->date_picker.date.day != picker->date_picker.date.day;
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmDatePickerWidget picker = (XmDatePickerWidget)widget;
    char text[32];
    (void)event; (void)region;
    if (!XtIsRealized(widget) || !picker->date_picker.gc) return;
    XClearWindow(XtDisplay(widget), XtWindow(widget));
    snprintf(text, sizeof(text), "%04d-%02d-%02d",
             picker->date_picker.date.year, picker->date_picker.date.month,
             picker->date_picker.date.day);
    XDrawString(XtDisplay(widget), XtWindow(widget), picker->date_picker.gc,
                6, 18, text, (int)strlen(text));
}

static void Events(Widget widget, XtPointer closure, XEvent *event,
                   Boolean *continue_dispatch) {
    XmDatePickerWidget picker = (XmDatePickerWidget)widget;
    KeySym key;
    char input[2];
    (void)closure; (void)continue_dispatch;
    if (event->type == ButtonPress ||
        (event->type == KeyPress &&
         (XLookupString(&event->xkey, input, sizeof(input), &key, NULL) >= 0) &&
         (key == XK_Return || key == XK_KP_Enter || key == XK_space))) {
        if (picker->date_picker.open)
            XmDatePickerClose(widget);
        else
            XmDatePickerOpen(widget);
    }
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    (void)widget; (void)request;
    reply->request_mode = CWWidth | CWHeight;
    reply->width = 120; reply->height = 28;
    return XtGeometryAlmost;
}

Widget XmCreateDatePicker(Widget parent, char *name, ArgList args,
                          Cardinal count) {
    return XtCreateWidget(name, xmDatePickerWidgetClass, parent, args, count);
}

Boolean XmDatePickerGetDate(Widget widget, XmDatePickerDate *date_return) {
    if (!widget || !date_return || !XmIsDatePicker(widget)) return False;
    *date_return = ((XmDatePickerWidget)widget)->date_picker.date;
    return True;
}

Boolean XmDatePickerSetDate(Widget widget, const XmDatePickerDate *date,
                            Boolean notify_change) {
    XmDatePickerWidget picker;
    if (!widget || !date || !XmIsDatePicker(widget) ||
        !XmCalendarIsValidDate(date)) return False;
    picker = (XmDatePickerWidget)widget;
    if (compare_dates(date, &picker->date_picker.minimum) < 0 ||
        compare_dates(date, &picker->date_picker.maximum) > 0) return False;
    if (picker->date_picker.calendar &&
        !XmCalendarSetDate(picker->date_picker.calendar, date, False))
        return False;
    if ((date->year == picker->date_picker.date.year) &&
        (date->month == picker->date_picker.date.month) &&
        (date->day == picker->date_picker.date.day)) return True;
    picker->date_picker.date = *date;
    if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget), 0,0,0,0,True);
    if (notify_change) notify(picker, NULL, 1, XmNdatePickerDateChangedCallback);
    return True;
}

Boolean XmDatePickerSetRange(Widget widget, const XmDatePickerDate *minimum,
                             const XmDatePickerDate *maximum) {
    XmDatePickerWidget picker;
    if (!widget || !XmIsDatePicker(widget) || !XmCalendarIsValidDate(minimum) ||
        !XmCalendarIsValidDate(maximum) ||
        compare_dates(minimum, maximum) > 0) return False;
    picker = (XmDatePickerWidget)widget;
    if (compare_dates(&picker->date_picker.date, minimum) < 0 ||
        compare_dates(&picker->date_picker.date, maximum) > 0) return False;
    picker->date_picker.minimum = *minimum;
    picker->date_picker.maximum = *maximum;
    if (picker->date_picker.calendar &&
        !XmCalendarSetRange(picker->date_picker.calendar, minimum, maximum))
        return False;
    return True;
}

Boolean XmDatePickerGetRange(Widget widget, XmDatePickerDate *minimum_return,
                             XmDatePickerDate *maximum_return) {
    XmDatePickerWidget picker;
    if (!widget || !XmIsDatePicker(widget) || !minimum_return ||
        !maximum_return) return False;
    picker = (XmDatePickerWidget)widget;
    *minimum_return = picker->date_picker.minimum;
    *maximum_return = picker->date_picker.maximum;
    return True;
}

Boolean XmDatePickerIsOpen(Widget widget) {
    return widget && XmIsDatePicker(widget) &&
           ((XmDatePickerWidget)widget)->date_picker.open;
}

void XmDatePickerOpen(Widget widget) {
    XmDatePickerWidget picker;
    if (!widget || !XmIsDatePicker(widget)) return;
    picker = (XmDatePickerWidget)widget;
    if (picker->date_picker.open || !XtIsRealized(widget)) return;
    create_popup(picker);
    if (!picker->date_picker.popup_shell) return;
    XmCalendarSetRange(picker->date_picker.calendar,
                       &picker->date_picker.minimum,
                       &picker->date_picker.maximum);
    XmCalendarSetDate(picker->date_picker.calendar,
                      &picker->date_picker.date, False);
    position_popup(picker);
    XtPopup(picker->date_picker.popup_shell, XtGrabNone);
    picker->date_picker.open = True;
    notify(picker, NULL, 3, XmNdatePickerOpenChangedCallback);
}

void XmDatePickerClose(Widget widget) {
    XmDatePickerWidget picker;
    if (!widget || !XmIsDatePicker(widget)) return;
    picker = (XmDatePickerWidget)widget;
    if (!picker->date_picker.open) return;
    XtPopdown(picker->date_picker.popup_shell);
    picker->date_picker.open = False;
    notify(picker, NULL, 4, XmNdatePickerOpenChangedCallback);
}
