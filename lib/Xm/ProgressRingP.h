#ifndef _XmProgressRingP_h
#define _XmProgressRingP_h

#include <Xm/ProgressRing.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmProgressRingClassPart { XtPointer extension; }
    XmProgressRingClassPart;
typedef struct _XmProgressRingClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmProgressRingClassPart progress_ring_class;
} XmProgressRingClassRec;

typedef struct _XmProgressRingPart {
    int minimum, maximum, value;
    Boolean indeterminate, text_visible;
    XmString label;
    XmFontList font;
    XtCallbackList value_changed_callback;
    Dimension spacing;
    Boolean width_default;
    Boolean height_default;
    GC track_gc, fill_gc, text_gc;
} XmProgressRingPart;

typedef struct _XmProgressRingRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmProgressRingPart progress_ring;
} XmProgressRingRec;

extern XmProgressRingClassRec xmProgressRingClassRec;

#endif /* _XmProgressRingP_h */
