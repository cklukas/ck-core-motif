#ifndef _XmIconLabelP_h
#define _XmIconLabelP_h

#include <Xm/IconLabel.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmIconLabelClassPart { XtPointer extension; } XmIconLabelClassPart;
typedef struct _XmIconLabelClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmIconLabelClassPart icon_label_class;
} XmIconLabelClassRec;

typedef struct _XmIconLabelPart {
    XmString label;
    Pixmap pixmap;
    Dimension spacing;
    GC foreground_gc;
} XmIconLabelPart;

typedef struct _XmIconLabelRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmIconLabelPart icon_label;
} XmIconLabelRec;

extern XmIconLabelClassRec xmIconLabelClassRec;

#endif /* _XmIconLabelP_h */
