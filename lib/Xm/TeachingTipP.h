#ifndef _XmTeachingTipP_h
#define _XmTeachingTipP_h

#include <Xm/TeachingTip.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmTeachingTipClassPart { XtPointer extension; }
    XmTeachingTipClassPart;
typedef struct _XmTeachingTipClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmTeachingTipClassPart teaching_tip_class;
} XmTeachingTipClassRec;

typedef struct _XmTeachingTipPart {
    XmString title;
    XmString text;
    Position offset_x, offset_y;
    unsigned int delay;
    XtCallbackList dismiss_callback;
    Widget owner, popup_shell, title_label, text_label;
    XtIntervalId timeout_id;
    Boolean timeout_pending, visible;
} XmTeachingTipPart;

typedef struct _XmTeachingTipRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmTeachingTipPart teaching_tip;
} XmTeachingTipRec;

extern XmTeachingTipClassRec xmTeachingTipClassRec;

#endif /* _XmTeachingTipP_h */
