/* Additive date-only calendar control.  LGPL-2.0-or-later. */
#ifndef _XmCalendar_h
#define _XmCalendar_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmCalendarWidgetClass;
typedef struct _XmCalendarClassRec *XmCalendarWidgetClass;
typedef struct _XmCalendarRec *XmCalendarWidget;

#ifndef XmIsCalendar
#define XmIsCalendar(w) XtIsSubclass((w), xmCalendarWidgetClass)
#endif

/* A calendar date is a civil date, not an instant and has no time zone. */
typedef struct {
    int year;                 /* 1 through 9999 */
    int month;                /* 1 through 12 */
    int day;                  /* 1 through days-in-month */
} XmCalendarDate;

typedef struct {
    int reason;
    XEvent *event;
    XmCalendarDate date;
} XmCalendarCallbackStruct;

Widget XmCreateCalendar(Widget parent, char *name, ArgList args,
                        Cardinal count);
Boolean XmCalendarIsValidDate(const XmCalendarDate *date);
Boolean XmCalendarGetDate(Widget widget, XmCalendarDate *date_return);
Boolean XmCalendarSetDate(Widget widget, const XmCalendarDate *date,
                          Boolean notify);
Boolean XmCalendarGetRange(Widget widget, XmCalendarDate *minimum_return,
                            XmCalendarDate *maximum_return);
Boolean XmCalendarSetRange(Widget widget, const XmCalendarDate *minimum,
                           const XmCalendarDate *maximum);
Boolean XmCalendarGetVisibleMonth(Widget widget, int *year_return,
                                  int *month_return);
Boolean XmCalendarSetVisibleMonth(Widget widget, int year, int month);

#ifdef __cplusplus
}
#endif

#endif /* _XmCalendar_h */
