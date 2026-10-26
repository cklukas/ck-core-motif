#ifndef _XmTimePickerP_h
#define _XmTimePickerP_h

#include <Xm/TimePicker.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmTimePickerClassPart { XtPointer extension; }
    XmTimePickerClassPart;
typedef struct _XmTimePickerClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmTimePickerClassPart time_picker_class;
} XmTimePickerClassRec;

typedef struct _XmTimePickerPart {
    XmTimePickerValue value, minimum, maximum;
    unsigned char field;
    Boolean format_24_hour;
    XtCallbackList value_changed_callback, activate_callback;
    XmFontList render_table;
    Boolean check_set_render_table;
    GC gc;
} XmTimePickerPart;

typedef struct _XmTimePickerRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmTimePickerPart time_picker;
} XmTimePickerRec;

extern XmTimePickerClassRec xmTimePickerClassRec;

#endif /* _XmTimePickerP_h */
