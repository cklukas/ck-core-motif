/* Additive date-only picker backed by XmCalendar. */
#ifndef _XmDatePicker_h
#define _XmDatePicker_h

#include <Xm/Calendar.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmDatePickerWidgetClass;
typedef struct _XmDatePickerClassRec *XmDatePickerWidgetClass;
typedef struct _XmDatePickerRec *XmDatePickerWidget;

#ifndef XmIsDatePicker
#define XmIsDatePicker(w) XtIsSubclass((w), xmDatePickerWidgetClass)
#endif

typedef XmCalendarDate XmDatePickerDate;
typedef struct {
    int reason;
    XEvent *event;
    XmDatePickerDate date;
} XmDatePickerCallbackStruct;

Widget XmCreateDatePicker(Widget parent, char *name, ArgList args,
                          Cardinal count);
Boolean XmDatePickerGetDate(Widget widget, XmDatePickerDate *date_return);
Boolean XmDatePickerSetDate(Widget widget, const XmDatePickerDate *date,
                            Boolean notify);
Boolean XmDatePickerSetRange(Widget widget, const XmDatePickerDate *minimum,
                             const XmDatePickerDate *maximum);
Boolean XmDatePickerGetRange(Widget widget, XmDatePickerDate *minimum_return,
                             XmDatePickerDate *maximum_return);
Boolean XmDatePickerIsOpen(Widget widget);
void XmDatePickerOpen(Widget widget);
void XmDatePickerClose(Widget widget);

#ifdef __cplusplus
}
#endif

#endif /* _XmDatePicker_h */
