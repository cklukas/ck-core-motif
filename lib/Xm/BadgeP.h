#ifndef _XmBadgeP_h
#define _XmBadgeP_h
#include <Xm/Badge.h>
#include <Xm/PrimitiveP.h>
typedef struct _XmBadgeClassRec { CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class; XtPointer extension; } XmBadgeClassRec;
typedef struct _XmBadgePart { XmString label; int count; int maximum;
    unsigned char severity; GC foreground_gc; } XmBadgePart;
typedef struct _XmBadgeRec { CorePart core; XmPrimitivePart primitive;
    XmBadgePart badge; } XmBadgeRec;
extern XmBadgeClassRec xmBadgeClassRec;
#endif
