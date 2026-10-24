/* Additive wall-clock time picker. */
#ifndef _XmTimePicker_h
#define _XmTimePicker_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmTimePickerWidgetClass;
typedef struct _XmTimePickerClassRec *XmTimePickerWidgetClass;
typedef struct _XmTimePickerRec *XmTimePickerWidget;

#ifndef XmIsTimePicker
#define XmIsTimePicker(w) XtIsSubclass((w), xmTimePickerWidgetClass)
#endif

/* A wall-clock value has no date, time zone, DST rule, or instant meaning. */
typedef struct {
    int hour;                 /* 0 through 23 */
    int minute;               /* 0 through 59 */
    int second;               /* 0 through 59 */
} XmTimePickerValue;

typedef struct {
    int reason;
    XEvent *event;
    XmTimePickerValue value;
} XmTimePickerCallbackStruct;

Widget XmCreateTimePicker(Widget parent, char *name, ArgList args,
                          Cardinal count);
Boolean XmTimePickerIsValid(const XmTimePickerValue *value);
Boolean XmTimePickerGetValue(Widget widget, XmTimePickerValue *value_return);
Boolean XmTimePickerSetValue(Widget widget, const XmTimePickerValue *value,
                             Boolean notify);
Boolean XmTimePickerGetRange(Widget widget, XmTimePickerValue *minimum_return,
                             XmTimePickerValue *maximum_return);
Boolean XmTimePickerSetRange(Widget widget,
                             const XmTimePickerValue *minimum,
                             const XmTimePickerValue *maximum);

#ifdef __cplusplus
}
#endif

#endif /* _XmTimePicker_h */
