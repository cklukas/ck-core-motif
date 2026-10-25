#ifndef _XmToastP_h
#define _XmToastP_h
#include <Xm/Toast.h>
#include <Xm/ManagerP.h>
typedef struct _XmToastClassPart { XtPointer extension; } XmToastClassPart;
typedef struct _XmToastClassRec { CoreClassPart core_class; CompositeClassPart composite_class;
    ConstraintClassPart constraint_class; XmManagerClassPart manager_class;
    XmToastClassPart toast_class; } XmToastClassRec;
typedef struct _XmToastPart { XmString message, action_label; unsigned char severity;
    unsigned long timeout; XtCallbackList action_callback, dismiss_callback;
    Widget message_widget, action_widget; XtIntervalId timer; Boolean shown, paused,
    laying_out, initializing; } XmToastPart;
typedef struct _XmToastRec { CorePart core; CompositePart composite; ConstraintPart constraint;
    XmManagerPart manager; XmToastPart toast; } XmToastRec;
extern XmToastClassRec xmToastClassRec;
#endif
