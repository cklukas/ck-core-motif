#ifndef _XmNumberBoxP_h
#define _XmNumberBoxP_h

#include <Xm/NumberBox.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmNumberBoxClassPart { XtPointer extension; }
    XmNumberBoxClassPart;
typedef struct _XmNumberBoxClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmNumberBoxClassPart number_box_class;
} XmNumberBoxClassRec;

typedef struct _XmNumberBoxPart {
    int minimum, maximum, value, step, page_step;
    Boolean wrap, invalid;
    char text[64];
    XtCallbackList value_changed_callback, invalid_callback;
    Dimension spacing;
    GC text_gc, arrow_gc, invalid_gc;
    XFontStruct *font;
} XmNumberBoxPart;

typedef struct _XmNumberBoxRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmNumberBoxPart number_box;
} XmNumberBoxRec;

extern XmNumberBoxClassRec xmNumberBoxClassRec;

#endif /* _XmNumberBoxP_h */
