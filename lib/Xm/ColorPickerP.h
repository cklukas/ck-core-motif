#ifndef _XmColorPickerP_h
#define _XmColorPickerP_h
#include <Xm/ColorPicker.h>
#include <Xm/PrimitiveP.h>
typedef struct _XmColorPickerClassPart { XtPointer extension; } XmColorPickerClassPart;
typedef struct _XmColorPickerClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmColorPickerClassPart color_picker_class;
} XmColorPickerClassRec;
typedef struct _XmColorPickerPart {
    unsigned long value, preview;
    XtCallbackList preview_callback, value_changed_callback;
    GC gc;
} XmColorPickerPart;
typedef struct _XmColorPickerRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmColorPickerPart color_picker;
} XmColorPickerRec;
extern XmColorPickerClassRec xmColorPickerClassRec;
#endif /* _XmColorPickerP_h */
