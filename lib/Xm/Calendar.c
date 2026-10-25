#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <Xm/CalendarP.h>
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

#define Offset(field) XtOffsetOf(XmCalendarRec, field)
static XtResource resources[] = {
    {XmNcalendarYear, XmCCalendarYear, XmRInt, sizeof(int),
     Offset(calendar.date.year), XmRImmediate, (XtPointer)1970},
    {XmNcalendarMonth, XmCCalendarMonth, XmRInt, sizeof(int),
     Offset(calendar.date.month), XmRImmediate, (XtPointer)1},
    {XmNcalendarDay, XmCCalendarDay, XmRInt, sizeof(int),
     Offset(calendar.date.day), XmRImmediate, (XtPointer)1},
    {XmNcalendarWeekStart, XmCCalendarWeekStart, XmRUnsignedChar,
     sizeof(unsigned char), Offset(calendar.week_start), XmRImmediate,
     (XtPointer)0},
    {XmNcalendarRtl, XmCCalendarRtl, XmRBoolean, sizeof(Boolean),
     Offset(calendar.rtl), XmRImmediate, (XtPointer)False},
    {XmNcalendarDateChangedCallback, XmCCalendarDateChangedCallback,
     XmRCallback, sizeof(XtCallbackList),
     Offset(calendar.date_changed_callback), XmRPointer, (XtPointer)NULL},
    {XmNcalendarActivateCallback, XmCCalendarActivateCallback,
     XmRCallback, sizeof(XtCallbackList),
     Offset(calendar.activate_callback), XmRPointer, (XtPointer)NULL},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};

externaldef(xmcalendarclassrec) XmCalendarClassRec xmCalendarClassRec =
    {{(WidgetClass)&xmPrimitiveClassRec, "XmCalendar", sizeof(XmCalendarRec),
      NULL, NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0,
      resources, XtNumber(resources), NULLQUARK, True,
      XtExposeCompressMaximal, True, False, Destroy, NULL, Redisplay,
      SetValues, NULL, XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
      XtInheritTranslations, QueryGeometry, NULL, NULL},
     {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
      XtInheritTranslations, NULL, NULL, 0, NULL},
     {NULL}};

externaldef(xmcalendarwidgetclass) WidgetClass xmCalendarWidgetClass =
    (WidgetClass)&xmCalendarClassRec;

static Boolean is_leap(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

static int days_in_month(int year, int month) {
    static const int days[] = {31, 28, 31, 30, 31, 30,
                               31, 31, 30, 31, 30, 31};
    if (month == 2 && is_leap(year))
        return 29;
    return days[month - 1];
}

Boolean XmCalendarIsValidDate(const XmCalendarDate *date) {
    return date && date->year >= 1 && date->year <= 9999 &&
           date->month >= 1 && date->month <= 12 && date->day >= 1 &&
           date->day <= days_in_month(date->year, date->month);
}

static int compare_dates(const XmCalendarDate *a, const XmCalendarDate *b) {
    if (a->year != b->year)
        return a->year < b->year ? -1 : 1;
    if (a->month != b->month)
        return a->month < b->month ? -1 : 1;
    if (a->day != b->day)
        return a->day < b->day ? -1 : 1;
    return 0;
}

static int days_from_civil(XmCalendarDate date) {
    /* Rata Die-like ordinal, valid for the documented 1..9999 range. */
    int year = date.year - 1;
    int before_month = (367 * date.month - 362) / 12;
    int correction = date.month <= 2 ? 0 : (is_leap(date.year) ? -1 : -2);
    return 365 * year + year / 4 - year / 100 + year / 400 +
           before_month + correction + date.day;
}

static XmCalendarDate date_from_days(int ordinal) {
    XmCalendarDate result;
    int low = 1, high = 9999;
    while (low < high) {
        int mid = low + (high - low + 1) / 2;
        XmCalendarDate probe = {mid, 12, 31};
        if (days_from_civil(probe) <= ordinal)
            low = mid;
        else
            high = mid - 1;
    }
    result.year = low;
    result.month = 1;
    result.day = 1;
    while (result.month < 12 &&
           days_from_civil((XmCalendarDate){result.year,
                                            result.month + 1, 1}) <= ordinal)
        ++result.month;
    result.day = ordinal - days_from_civil(result) + 1;
    return result;
}

static XmCalendarDate add_days(XmCalendarDate date, int delta) {
    return date_from_days(days_from_civil(date) + delta);
}

static void normalize_visible(XmCalendarWidget calendar) {
    calendar->calendar.visible_year = calendar->calendar.date.year;
    calendar->calendar.visible_month = calendar->calendar.date.month;
}

static int text_baseline(XmCalendarWidget calendar, int top, int height) {
    XGCValues values;
    XFontStruct *font;
    long long baseline;
    if (!calendar->calendar.gc ||
        !XGetGCValues(XtDisplay((Widget)calendar), calendar->calendar.gc,
                      GCFont, &values))
        return top + height / 2;
    font = XQueryFont(XtDisplay((Widget)calendar), values.font);
    if (!font)
        return top + height / 2;
    baseline = (long long)top +
        ((long long)height - font->ascent - font->descent) / 2 + font->ascent;
    XFreeFontInfo(NULL, font, 1);
    return baseline < INT_MIN ? INT_MIN :
           baseline > INT_MAX ? INT_MAX : (int)baseline;
}

static void notify_date(XmCalendarWidget calendar, XEvent *event,
                        String callback_name, int reason) {
    XmCalendarCallbackStruct callback;
    XtCallbackList list = callback_name &&
        strcmp(callback_name, XmNcalendarActivateCallback) == 0
        ? calendar->calendar.activate_callback
        : calendar->calendar.date_changed_callback;
    if (!list)
        return;
    callback.reason = reason;
    callback.event = event;
    callback.date = calendar->calendar.date;
    XtCallCallbacks((Widget)calendar, callback_name, &callback);
}

static Boolean date_in_range(XmCalendarWidget calendar,
                             const XmCalendarDate *date) {
    return compare_dates(date, &calendar->calendar.minimum) >= 0 &&
           compare_dates(date, &calendar->calendar.maximum) <= 0;
}

static Boolean set_date_internal(XmCalendarWidget calendar,
                                 const XmCalendarDate *date, Boolean notify,
                                 XEvent *event) {
    if (!XmCalendarIsValidDate(date) || !date_in_range(calendar, date) ||
        compare_dates(date, &calendar->calendar.date) == 0)
        return False;
    calendar->calendar.date = *date;
    normalize_visible(calendar);
    if (XtIsRealized((Widget)calendar))
        XClearArea(XtDisplay((Widget)calendar), XtWindow((Widget)calendar),
                   0, 0, 0, 0, True);
    if (notify)
        notify_date(calendar, event, XmNcalendarDateChangedCallback, 1);
    return True;
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmCalendarWidget calendar = (XmCalendarWidget)widget;
    XGCValues values;
    XmCalendarDate initial;
    (void)request; (void)args; (void)count;
    initial = calendar->calendar.date;
    if (!XmCalendarIsValidDate(&initial)) {
        initial.year = 1970; initial.month = 1; initial.day = 1;
        calendar->calendar.date = initial;
    }
    calendar->calendar.minimum = (XmCalendarDate){1, 1, 1};
    calendar->calendar.maximum = (XmCalendarDate){9999, 12, 31};
    normalize_visible(calendar);
    if (calendar->calendar.week_start > 6)
        calendar->calendar.week_start = 0;
    values.foreground = calendar->primitive.foreground;
    values.background = calendar->core.background_pixel;
    calendar->calendar.gc = XCreateGC(XtDisplay(widget),
                                      DefaultRootWindow(XtDisplay(widget)),
                                      GCForeground | GCBackground, &values);
    if (!calendar->core.width)
        calendar->core.width = 224;
    if (!calendar->core.height)
        calendar->core.height = 192;
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events,
                      NULL);
}

static void Destroy(Widget widget) {
    XmCalendarWidget calendar = (XmCalendarWidget)widget;
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask, False,
                         Events, NULL);
    if (calendar->calendar.gc)
        XFreeGC(XtDisplay(widget), calendar->calendar.gc);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget new_widget,
                         ArgList args, Cardinal *count) {
    XmCalendarWidget old_calendar = (XmCalendarWidget)old_widget;
    XmCalendarWidget calendar = (XmCalendarWidget)new_widget;
    XmCalendarDate candidate = calendar->calendar.date;
    Boolean redraw = False;
    (void)request; (void)args; (void)count;
    if (!XmCalendarIsValidDate(&candidate) ||
        !date_in_range(old_calendar, &candidate)) {
        calendar->calendar.date = old_calendar->calendar.date;
        normalize_visible(calendar);
    } else if (compare_dates(&candidate, &old_calendar->calendar.date) != 0) {
        normalize_visible(calendar);
        redraw = True;
    }
    if (calendar->calendar.week_start > 6) {
        calendar->calendar.week_start = old_calendar->calendar.week_start;
        redraw = True;
    }
    redraw = redraw || calendar->calendar.rtl != old_calendar->calendar.rtl;
    return redraw;
}

static int first_weekday(int year, int month) {
    XmCalendarDate date = {year, month, 1};
    /* 0 = Sunday, matching X11's traditional calendar convention. */
    return days_from_civil(date) % 7;
}

static const char *month_name(int month) {
    static const char *names[] = {"January", "February", "March", "April",
                                  "May", "June", "July", "August",
                                  "September", "October", "November",
                                  "December"};
    return names[month - 1];
}

static void draw_day(XmCalendarWidget calendar, int col, int row, int day,
                     Boolean enabled, Boolean selected) {
    Display *display = XtDisplay((Widget)calendar);
    Window window = XtWindow((Widget)calendar);
    int cell_width = (int)calendar->core.width / 7;
    int x = col * cell_width + 4;
    if (!calendar->calendar.gc || cell_width < 1)
        return;
    int top = 42 + row * 22;
    char text[12];
    snprintf(text, sizeof(text), "%2d", day);
    if (selected)
        XDrawRectangle(display, window, calendar->calendar.gc, x - 2, top + 1,
                       (unsigned)(cell_width > 5 ? cell_width - 5 : 1), 20);
    if (enabled)
        XDrawString(display, window, calendar->calendar.gc, x,
                    text_baseline(calendar, top, 22), text, 2);
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmCalendarWidget calendar = (XmCalendarWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    int first = first_weekday(calendar->calendar.visible_year,
                              calendar->calendar.visible_month);
    int offset = (first - calendar->calendar.week_start + 7) % 7;
    int days = days_in_month(calendar->calendar.visible_year,
                             calendar->calendar.visible_month);
    int day, index;
    char heading[64];
    static const char *weekdays[] = {"Su", "Mo", "Tu", "We", "Th", "Fr",
                                     "Sa"};
    (void)event; (void)region;
    if (!XtIsRealized(widget) || !calendar->calendar.gc ||
        calendar->core.width < 7)
        return;
    XClearWindow(display, window);
    snprintf(heading, sizeof(heading), "%s %d", month_name(calendar->calendar.visible_month),
             calendar->calendar.visible_year);
    XDrawString(display, window, calendar->calendar.gc, 6,
                text_baseline(calendar, 0, 28), heading,
                (int)strlen(heading));
    for (index = 0; index < 7; ++index) {
        int col = calendar->calendar.rtl ? 6 - index : index;
        int weekday = (calendar->calendar.week_start + index) % 7;
        XDrawString(display, window, calendar->calendar.gc,
                    col * ((int)calendar->core.width / 7) + 5,
                    text_baseline(calendar, 28, 14), weekdays[weekday], 2);
    }
    for (day = 1; day <= days; ++day) {
        XmCalendarDate candidate = {calendar->calendar.visible_year,
                                    calendar->calendar.visible_month, day};
        int position = offset + day - 1;
        int col = position % 7;
        int row = position / 7;
        if (calendar->calendar.rtl)
            col = 6 - col;
        draw_day(calendar, col, row, day, date_in_range(calendar, &candidate),
                 compare_dates(&candidate, &calendar->calendar.date) == 0);
    }
}

static Boolean month_allowed(XmCalendarWidget calendar, int year, int month) {
    XmCalendarDate first = {year, month, 1};
    XmCalendarDate last = {year, month, days_in_month(year, month)};
    return XmCalendarIsValidDate(&first) &&
           compare_dates(&last, &calendar->calendar.minimum) >= 0 &&
           compare_dates(&first, &calendar->calendar.maximum) <= 0;
}

static Boolean set_visible_month_internal(XmCalendarWidget calendar, int year,
                                          int month, XEvent *event) {
    if (year < 1 || year > 9999 || month < 1 || month > 12 ||
        !month_allowed(calendar, year, month))
        return False;
    if (calendar->calendar.visible_year == year &&
        calendar->calendar.visible_month == month)
        return True;
    calendar->calendar.visible_year = year;
    calendar->calendar.visible_month = month;
    if (XtIsRealized((Widget)calendar))
        XClearArea(XtDisplay((Widget)calendar), XtWindow((Widget)calendar),
                   0, 0, 0, 0, True);
    (void)event;
    return True;
}

static void activate(XmCalendarWidget calendar, XEvent *event) {
    notify_date(calendar, event, XmNcalendarActivateCallback, 2);
}

static void Events(Widget widget, XtPointer closure, XEvent *event,
                   Boolean *continue_dispatch) {
    XmCalendarWidget calendar = (XmCalendarWidget)widget;
    XmCalendarDate candidate;
    KeySym key;
    char buffer[8];
    int col, row, first, offset, position, day;
    (void)closure; (void)continue_dispatch;
    if (event->type == KeyPress) {
        XLookupString(&event->xkey, buffer, sizeof(buffer), &key, NULL);
        candidate = calendar->calendar.date;
        if (key == XK_Left || key == XK_Right || key == XK_Up ||
            key == XK_Down) {
            int delta = key == XK_Left ? -1 : key == XK_Right ? 1 :
                        key == XK_Up ? -7 : 7;
            if (calendar->calendar.rtl && (key == XK_Left || key == XK_Right))
                delta = -delta;
            candidate = add_days(candidate, delta);
        } else if (key == XK_Page_Up || key == XK_Page_Down) {
            int month = candidate.month + (key == XK_Page_Up ? -1 : 1);
            int year = candidate.year;
            while (month < 1) { month += 12; --year; }
            while (month > 12) { month -= 12; ++year; }
            candidate.month = month;
            candidate.year = year;
            if (candidate.day > days_in_month(year, month))
                candidate.day = days_in_month(year, month);
        } else if (key == XK_Home) {
            candidate.day = 1;
        } else if (key == XK_End) {
            candidate.day = days_in_month(candidate.year, candidate.month);
        } else if (key == XK_Return || key == XK_KP_Enter) {
            activate(calendar, event);
            return;
        } else {
            return;
        }
        set_date_internal(calendar, &candidate, True, event);
        return;
    }
    if (event->type != ButtonPress || event->xbutton.y < 40)
        return;
    {
        int cell_width = (int)calendar->core.width / 7;
        if (cell_width < 1)
            return;
        col = event->xbutton.x / cell_width;
    }
    row = (event->xbutton.y - 40) / 22;
    if (col < 0 || col > 6 || row < 0 || row > 5)
        return;
    if (calendar->calendar.rtl)
        col = 6 - col;
    first = first_weekday(calendar->calendar.visible_year,
                          calendar->calendar.visible_month);
    offset = (first - calendar->calendar.week_start + 7) % 7;
    position = row * 7 + col;
    day = position - offset + 1;
    if (day < 1 || day > days_in_month(calendar->calendar.visible_year,
                                       calendar->calendar.visible_month))
        return;
    candidate = (XmCalendarDate){calendar->calendar.visible_year,
                                calendar->calendar.visible_month, day};
    set_date_internal(calendar, &candidate, True, event);
    if (compare_dates(&candidate, &calendar->calendar.date) == 0)
        activate(calendar, event);
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *intended,
                                      XtWidgetGeometry *preferred) {
    (void)widget; (void)intended;
    preferred->request_mode = CWWidth | CWHeight;
    preferred->width = 224;
    preferred->height = 192;
    return XtGeometryAlmost;
}

Widget XmCreateCalendar(Widget parent, char *name, ArgList args,
                        Cardinal count) {
    return XtCreateWidget(name, xmCalendarWidgetClass, parent, args, count);
}

Boolean XmCalendarGetDate(Widget widget, XmCalendarDate *date_return) {
    XmCalendarWidget calendar;
    if (!widget || !date_return || !XmIsCalendar(widget))
        return False;
    calendar = (XmCalendarWidget)widget;
    *date_return = calendar->calendar.date;
    return True;
}

Boolean XmCalendarSetDate(Widget widget, const XmCalendarDate *date,
                          Boolean notify) {
    if (!widget || !XmIsCalendar(widget))
        return False;
    return set_date_internal((XmCalendarWidget)widget, date, notify, NULL);
}

Boolean XmCalendarGetRange(Widget widget, XmCalendarDate *minimum_return,
                           XmCalendarDate *maximum_return) {
    XmCalendarWidget calendar;
    if (!widget || !minimum_return || !maximum_return || !XmIsCalendar(widget))
        return False;
    calendar = (XmCalendarWidget)widget;
    *minimum_return = calendar->calendar.minimum;
    *maximum_return = calendar->calendar.maximum;
    return True;
}

Boolean XmCalendarSetRange(Widget widget, const XmCalendarDate *minimum,
                           const XmCalendarDate *maximum) {
    XmCalendarWidget calendar;
    if (!widget || !XmIsCalendar(widget) || !XmCalendarIsValidDate(minimum) ||
        !XmCalendarIsValidDate(maximum) || compare_dates(minimum, maximum) > 0)
        return False;
    calendar = (XmCalendarWidget)widget;
    if (compare_dates(&calendar->calendar.date, minimum) < 0 ||
        compare_dates(&calendar->calendar.date, maximum) > 0)
        return False;
    calendar->calendar.minimum = *minimum;
    calendar->calendar.maximum = *maximum;
    if (!month_allowed(calendar, calendar->calendar.visible_year,
                       calendar->calendar.visible_month))
        normalize_visible(calendar);
    if (XtIsRealized(widget))
        XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True);
    return True;
}

Boolean XmCalendarGetVisibleMonth(Widget widget, int *year_return,
                                  int *month_return) {
    XmCalendarWidget calendar;
    if (!widget || !year_return || !month_return || !XmIsCalendar(widget))
        return False;
    calendar = (XmCalendarWidget)widget;
    *year_return = calendar->calendar.visible_year;
    *month_return = calendar->calendar.visible_month;
    return True;
}

Boolean XmCalendarSetVisibleMonth(Widget widget, int year, int month) {
    if (!widget || !XmIsCalendar(widget))
        return False;
    return set_visible_month_internal((XmCalendarWidget)widget, year, month,
                                      NULL);
}
