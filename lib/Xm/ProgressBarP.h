#ifndef _XmProgressBarP_h
#define _XmProgressBarP_h

#include <Xm/ProgressBar.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmProgressBarClassPart { XtPointer extension; }
    XmProgressBarClassPart;
typedef struct _XmProgressBarClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmProgressBarClassPart progress_class;
} XmProgressBarClassRec;

typedef struct _XmProgressBarPart {
    int minimum, maximum, value;
    Boolean indeterminate, text_visible;
    XmString label;
    XmFontList font;
    XtCallbackList value_changed_callback;
    Dimension spacing;
    GC track_gc, fill_gc, text_gc;
} XmProgressBarPart;

typedef struct _XmProgressBarRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmProgressBarPart progress;
} XmProgressBarRec;

extern XmProgressBarClassRec xmProgressBarClassRec;

#endif /* _XmProgressBarP_h */
