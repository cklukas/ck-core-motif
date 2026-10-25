#ifndef _XmRowActionsP_h
#define _XmRowActionsP_h
#include <Xm/RowActions.h>
#include <Xm/ManagerP.h>
typedef struct _XmRowActionsClassPart { XtPointer extension; } XmRowActionsClassPart;
typedef struct _XmRowActionsClassRec { CoreClassPart core_class; CompositeClassPart composite_class;
    ConstraintClassPart constraint_class; XmManagerClassPart manager_class; XmRowActionsClassPart actions_class; } XmRowActionsClassRec;
typedef struct _XmRowActionsStoredItem { char *id; XmString label, shortcut; XtPointer user_data; Widget button; } XmRowActionsStoredItem;
typedef struct _XmRowActionsPart { XmRowActionsStoredItem *items; Cardinal item_count; unsigned char side; Boolean revealed, laying_out, initializing; XtCallbackList invoke_callback; } XmRowActionsPart;
typedef struct _XmRowActionsRec { CorePart core; CompositePart composite; ConstraintPart constraint; XmManagerPart manager; XmRowActionsPart actions; } XmRowActionsRec;
extern XmRowActionsClassRec xmRowActionsClassRec;
#endif
