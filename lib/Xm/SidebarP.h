#ifndef _XmSidebarP_h
#define _XmSidebarP_h

#include <Xm/ManagerP.h>
#include <Xm/Sidebar.h>

typedef struct _XmSidebarClassPart {
    XtPointer extension;
} XmSidebarClassPart;

typedef struct _XmSidebarClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart manager_class;
    XmSidebarClassPart sidebar_class;
} XmSidebarClassRec;

typedef struct _XmSidebarPart {
    Widget toggle_button;
    Widget content;
    Widget saved_focus;
    Dimension expanded_width;
    Dimension collapsed_width;
    Boolean expanded;
    Boolean laying_out;
    Boolean initializing;
    XtCallbackList toggle_callback;
} XmSidebarPart;

typedef struct _XmSidebarRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    XmSidebarPart sidebar;
} XmSidebarRec;

extern XmSidebarClassRec xmSidebarClassRec;

#endif /* _XmSidebarP_h */
