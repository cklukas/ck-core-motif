#ifndef _XmSegmentedControlP_h
#define _XmSegmentedControlP_h

#include <Xm/ManagerP.h>
#include <Xm/SegmentedControl.h>

typedef struct _XmSegmentedControlClassPart { XtPointer extension; }
    XmSegmentedControlClassPart;
typedef struct _XmSegmentedControlClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart manager_class;
    XmSegmentedControlClassPart segmented_class;
} XmSegmentedControlClassRec;

typedef struct {
    char *id;
    XmString label;
    Pixmap pixmap;
    Boolean enabled;
    Widget button;
} XmSegmentedControlItemRec;

typedef struct _XmSegmentedControlPart {
    XmSegmentedControlItemRec *items;
    Cardinal item_count;
    int selected;
    Boolean allow_clear;
    Boolean rtl;
    unsigned char overflow;
    Dimension spacing;
    XtCallbackList selection_changed_callback;
    Boolean rebuilding;
} XmSegmentedControlPart;

typedef struct _XmSegmentedControlRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    XmSegmentedControlPart segmented;
} XmSegmentedControlRec;

extern XmSegmentedControlClassRec xmSegmentedControlClassRec;

#endif /* _XmSegmentedControlP_h */
