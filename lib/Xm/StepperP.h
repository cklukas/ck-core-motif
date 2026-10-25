#ifndef _XmStepperP_h
#define _XmStepperP_h
#include <Xm/Stepper.h>
#include <Xm/PrimitiveP.h>
typedef struct _XmStepperClassPart { XtPointer extension; } XmStepperClassPart;
typedef struct _XmStepperClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmStepperClassPart stepper_class;
} XmStepperClassRec;
typedef struct _XmStepperPart {
    int minimum, maximum, value, step;
    Boolean wrap;
    XtCallbackList value_changed_callback;
    GC gc;
    XmRenderTable render_table;
    Boolean check_set_render_table;
} XmStepperPart;
typedef struct _XmStepperRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmStepperPart stepper;
} XmStepperRec;
extern XmStepperClassRec xmStepperClassRec;
#endif /* _XmStepperP_h */
