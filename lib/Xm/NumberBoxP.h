#ifndef _XmNumberBoxP_h
#define _XmNumberBoxP_h

#include <Xm/NumberBox.h>
#include <Xm/ManagerP.h>

typedef struct _XmNumberBoxClassPart { XtPointer extension; }
    XmNumberBoxClassPart;
typedef struct _XmNumberBoxClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart manager_class;
    XmNumberBoxClassPart number_box_class;
} XmNumberBoxClassRec;

typedef struct _XmNumberBoxPart {
    int minimum, maximum, value, step, page_step;
    Boolean wrap, invalid;
    Widget editor, increment, decrement, error_label;
    Boolean initializing, updating, automatic_width, automatic_height;
    XtWorkProcId layout_work;
    XtCallbackList value_changed_callback, invalid_callback;
    Dimension spacing;
    XmRenderTable render_table;
    Boolean check_set_render_table;
} XmNumberBoxPart;

typedef struct _XmNumberBoxRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    XmNumberBoxPart number_box;
} XmNumberBoxRec;

extern XmNumberBoxClassRec xmNumberBoxClassRec;

#endif /* _XmNumberBoxP_h */
