#ifndef _XmDatePickerP_h
#define _XmDatePickerP_h

#include <Xm/DatePicker.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmDatePickerClassPart { XtPointer extension; }
    XmDatePickerClassPart;
typedef struct _XmDatePickerClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmDatePickerClassPart date_picker_class;
} XmDatePickerClassRec;

typedef struct _XmDatePickerPart {
    XmDatePickerDate date, minimum, maximum;
    Boolean open;
    Widget popup_shell, calendar;
    XtCallbackList date_changed_callback, open_changed_callback;
    GC gc;
} XmDatePickerPart;

typedef struct _XmDatePickerRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmDatePickerPart date_picker;
} XmDatePickerRec;

extern XmDatePickerClassRec xmDatePickerClassRec;

#endif /* _XmDatePickerP_h */
