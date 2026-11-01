#ifndef _XmStepperP_h
#define _XmStepperP_h
#include <Xm/Stepper.h>
#include <Xm/ManagerP.h>
typedef struct _XmStepperClassPart { XtPointer extension; } XmStepperClassPart;
typedef struct _XmStepperClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart manager_class;
    XmStepperClassPart stepper_class;
} XmStepperClassRec;
typedef struct _XmStepperPart {
    int minimum, maximum, value, step;
    Boolean wrap;
    XtCallbackList value_changed_callback;
    Widget decrement, label, increment;
    Dimension spacing;
    Boolean initializing, updating, automatic_width, automatic_height;
    XtWorkProcId layout_work;
    XmRenderTable render_table;
    Boolean check_set_render_table;
} XmStepperPart;
typedef struct _XmStepperRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    XmStepperPart stepper;
} XmStepperRec;
extern XmStepperClassRec xmStepperClassRec;
#endif /* _XmStepperP_h */
