#ifndef _XmFlyoutP_h
#define _XmFlyoutP_h

#include <Xm/Flyout.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmFlyoutClassPart { XtPointer extension; }
    XmFlyoutClassPart;
typedef struct _XmFlyoutClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmFlyoutClassPart flyout_class;
} XmFlyoutClassRec;

typedef struct _XmFlyoutPart {
    String text;
    Position offset_x, offset_y;
    XtCallbackList dismiss_callback;
    Widget popup_shell, label, owner;
    Boolean visible;
} XmFlyoutPart;

typedef struct _XmFlyoutRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmFlyoutPart flyout;
} XmFlyoutRec;

extern XmFlyoutClassRec xmFlyoutClassRec;

#endif /* _XmFlyoutP_h */
