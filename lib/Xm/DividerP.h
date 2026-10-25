#ifndef _XmDividerP_h
#define _XmDividerP_h

#include <Xm/Divider.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmDividerClassPart { XtPointer extension; } XmDividerClassPart;
typedef struct _XmDividerClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmDividerClassPart divider_class;
} XmDividerClassRec;

typedef struct _XmDividerPart {
    XmString label;
    unsigned char orientation;
    Dimension thickness;
    Dimension spacing;
    GC foreground_gc;
} XmDividerPart;

typedef struct _XmDividerRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmDividerPart divider;
} XmDividerRec;

extern XmDividerClassRec xmDividerClassRec;

#endif /* _XmDividerP_h */
