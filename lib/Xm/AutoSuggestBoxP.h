#ifndef _XmAutoSuggestBoxP_h
#define _XmAutoSuggestBoxP_h

#include <Xm/ManagerP.h>
#include <Xm/AutoSuggestBox.h>

typedef struct _XmAutoSuggestBoxClassPart {
    XtPointer extension;
} XmAutoSuggestBoxClassPart;

typedef struct _XmAutoSuggestBoxClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart manager_class;
    XmAutoSuggestBoxClassPart auto_suggest_class;
} XmAutoSuggestBoxClassRec;

typedef struct _XmAutoSuggestStoredItem {
    char *id;
    XmString label;
    XtPointer user_data;
} XmAutoSuggestStoredItem;

typedef struct _XmAutoSuggestBoxPart {
    Widget search;
    Widget popup_shell;
    Widget list;
    Widget scroller;
    XtWorkProcId layout_work;
    XtIntervalId cancel_timer;
    XtIntervalId focus_timer;
    struct _XmAutoSuggestWatch *outside_watches;
    Boolean watching_outside;
    Boolean automatic_width, automatic_height;
    XmAutoSuggestStoredItem *items;
    Cardinal item_count;
    unsigned long generation;
    Boolean visible, pending_show, cancel_pending;
    Boolean no_results;
    String no_results_label;
    XtCallbackList query_changed_callback;
    XtCallbackList committed_callback;
    XtCallbackList dismissed_callback;
    Boolean laying_out;
    Boolean initializing;
    Boolean committing;
} XmAutoSuggestBoxPart;

typedef struct _XmAutoSuggestBoxRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    XmAutoSuggestBoxPart auto_suggest;
} XmAutoSuggestBoxRec;

extern XmAutoSuggestBoxClassRec xmAutoSuggestBoxClassRec;

#endif /* _XmAutoSuggestBoxP_h */
