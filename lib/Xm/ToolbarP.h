#ifndef _XmToolbarP_h
#define _XmToolbarP_h

#include <Xm/ManagerP.h>
#include <Xm/Toolbar.h>

typedef struct _XmToolbarClassPart {
    XtPointer extension;
} XmToolbarClassPart;

typedef struct _XmToolbarClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart manager_class;
    XmToolbarClassPart toolbar_class;
} XmToolbarClassRec;

typedef struct _XmToolbarStoredAction {
    char *id;
    XmString label;
    XmString shortcut;
    Boolean overflowable;
    unsigned char priority;
    XtPointer user_data;
    Widget button;
    Widget menu_button;
    Pixmap icon;
    Boolean visible;
} XmToolbarStoredAction;

typedef struct _XmToolbarPart {
    XmToolbarStoredAction *actions;
    Cardinal action_count;
    Cardinal visible_count;
    Cardinal overflow_count;
    Dimension spacing;
    String overflow_label;
    XtCallbackList activate_callback;
    Widget overflow_menu;
    Widget overflow_button;
    Boolean laying_out;
    Boolean initializing;
} XmToolbarPart;

typedef struct _XmToolbarRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    XmToolbarPart toolbar;
} XmToolbarRec;

extern XmToolbarClassRec xmToolbarClassRec;

#endif /* _XmToolbarP_h */
