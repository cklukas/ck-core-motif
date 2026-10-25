#ifndef _XmEmptyStateP_h
#define _XmEmptyStateP_h
#include <Xm/EmptyState.h>
#include <Xm/ManagerP.h>
typedef struct _XmEmptyStateClassPart { XtPointer extension; } XmEmptyStateClassPart;
typedef struct _XmEmptyStateClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart manager_class;
    XmEmptyStateClassPart empty_state_class;
} XmEmptyStateClassRec;
typedef struct _XmEmptyStatePart {
    Pixmap icon;
    XmString message, action_label;
    Dimension margin_width, margin_height, spacing;
    Widget icon_widget, message_widget, action_widget;
    Boolean laying_out, initializing;
    XtCallbackList action_callback;
} XmEmptyStatePart;
typedef struct _XmEmptyStateRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    XmEmptyStatePart empty_state;
} XmEmptyStateRec;
extern XmEmptyStateClassRec xmEmptyStateClassRec;
#endif
