#ifndef _XmChipP_h
#define _XmChipP_h

#include <Xm/Chip.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmChipClassPart { XtPointer extension; } XmChipClassPart;
typedef struct _XmChipClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmChipClassPart chip_class;
} XmChipClassRec;

typedef struct _XmChipPart {
    XmString label;
    XmFontList font;
    Pixmap pixmap;
    Boolean removable;
    XtCallbackList activate_callback;
    XtCallbackList remove_callback;
    GC foreground_gc;
    GC background_gc;
} XmChipPart;

typedef struct _XmChipRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmChipPart chip;
} XmChipRec;

extern XmChipClassRec xmChipClassRec;

#endif /* _XmChipP_h */
