#ifndef _XmDropHighlightP_h
#define _XmDropHighlightP_h

#include <Xm/DropHighlight.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmDropHighlightClassPart {
    XtPointer extension;
} XmDropHighlightClassPart;

typedef struct _XmDropHighlightClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmDropHighlightClassPart drop_highlight_class;
} XmDropHighlightClassRec;

typedef struct _XmDropHighlightPart {
    unsigned char state;
    Dimension thickness;
    XmString label;
    XmFontList font;
    Boolean check_set_render_table;
} XmDropHighlightPart;

typedef struct _XmDropHighlightRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmDropHighlightPart drop_highlight;
} XmDropHighlightRec;

extern XmDropHighlightClassRec xmDropHighlightClassRec;

#endif /* _XmDropHighlightP_h */
