#ifndef _XmSpinnerP_h
#define _XmSpinnerP_h

#include <Xm/Spinner.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmSpinnerClassPart { XtPointer extension; }
    XmSpinnerClassPart;
typedef struct _XmSpinnerClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmSpinnerClassPart spinner_class;
} XmSpinnerClassRec;

typedef struct _XmSpinnerPart {
    Boolean active;
    int interval;
    XmString label;
    XmFontList font;
    XtCallbackList state_changed_callback;
    XtIntervalId timer;
    Boolean timer_pending;
    unsigned int phase;
    Dimension spacing;
    GC ring_gc, text_gc;
} XmSpinnerPart;

typedef struct _XmSpinnerRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmSpinnerPart spinner;
} XmSpinnerRec;

extern XmSpinnerClassRec xmSpinnerClassRec;

#endif /* _XmSpinnerP_h */
