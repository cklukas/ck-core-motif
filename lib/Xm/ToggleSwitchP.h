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
    Dimension spacing;
    GC track_gc;
    Boolean check_set_render_table;
    Boolean automatic_width, automatic_height;
    Boolean armed, hover;
} XmToggleSwitchPart;

typedef struct _XmToggleSwitchRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmToggleSwitchPart toggle_switch;
} XmToggleSwitchRec;

extern XmToggleSwitchClassRec xmToggleSwitchClassRec;

#endif /* _XmToggleSwitchP_h */
