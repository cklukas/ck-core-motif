#ifndef _XmStatusBarP_h
#define _XmStatusBarP_h

#include <Xm/ManagerP.h>
#include <Xm/StatusBar.h>

typedef struct _XmStatusBarClassPart {
    XtPointer extension;
} XmStatusBarClassPart;

typedef struct _XmStatusBarClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart manager_class;
    XmStatusBarClassPart status_bar_class;
} XmStatusBarClassRec;

typedef struct _XmStatusBarStoredItem {
    char *id;
    XmString label;
    XmString display;
    unsigned char kind;
    unsigned char priority;
    int value;
    int maximum;
    XtPointer user_data;
    Widget child;
    Boolean visible;
} XmStatusBarStoredItem;

typedef struct _XmStatusBarPart {
    XmStatusBarStoredItem *items;
    Cardinal item_count;
    Cardinal visible_count;
    Cardinal overflow_count;
    Dimension spacing;
    Boolean laying_out;
    Boolean initializing;
} XmStatusBarPart;

typedef struct _XmStatusBarRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    XmStatusBarPart status_bar;
} XmStatusBarRec;

extern XmStatusBarClassRec xmStatusBarClassRec;

#endif /* _XmStatusBarP_h */
