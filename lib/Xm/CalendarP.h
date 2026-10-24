#ifndef _XmCalendarP_h
#define _XmCalendarP_h

#include <Xm/Calendar.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmCalendarClassPart { XtPointer extension; }
    XmCalendarClassPart;
typedef struct _XmCalendarClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmCalendarClassPart calendar_class;
} XmCalendarClassRec;

typedef struct _XmCalendarPart {
    XmCalendarDate date;
    XmCalendarDate minimum;
    XmCalendarDate maximum;
    int visible_year;
    int visible_month;
    unsigned char week_start;
    Boolean rtl;
    XtCallbackList date_changed_callback;
    XtCallbackList activate_callback;
    GC gc;
} XmCalendarPart;

typedef struct _XmCalendarRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmCalendarPart calendar;
} XmCalendarRec;

extern XmCalendarClassRec xmCalendarClassRec;

#endif /* _XmCalendarP_h */
