#ifndef _XmToggleSwitchP_h
#define _XmToggleSwitchP_h

#include <Xm/ToggleSwitch.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmToggleSwitchClassPart {
    XtPointer extension;
} XmToggleSwitchClassPart;

typedef struct _XmToggleSwitchClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmToggleSwitchClassPart switch_class;
} XmToggleSwitchClassRec;

typedef struct _XmToggleSwitchPart {
    Boolean set;
    XmString label;
    XmFontList font;
    XtCallbackList value_changed_callback;
    Pixel track_color;
    Pixel knob_color;
    Dimension spacing;
    GC track_gc;
    GC knob_gc;
    GC label_gc;
    GC insensitive_gc;
} XmToggleSwitchPart;

typedef struct _XmToggleSwitchRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmToggleSwitchPart toggle_switch;
} XmToggleSwitchRec;

extern XmToggleSwitchClassRec xmToggleSwitchClassRec;

#endif /* _XmToggleSwitchP_h */
