#ifndef _XmSkeletonP_h
#define _XmSkeletonP_h
#include <Xm/Skeleton.h>
#include <Xm/PrimitiveP.h>
typedef struct _XmSkeletonClassPart { XtPointer extension; } XmSkeletonClassPart;
typedef struct _XmSkeletonClassRec { CoreClassPart core_class; XmPrimitiveClassPart primitive_class; XmSkeletonClassPart skeleton_class; } XmSkeletonClassRec;
typedef struct _XmSkeletonPart { XmString label; Boolean animated, reduced_motion, running; unsigned long interval; XtIntervalId timer; GC gc; } XmSkeletonPart;
typedef struct _XmSkeletonRec { CorePart core; XmPrimitivePart primitive; XmSkeletonPart skeleton; } XmSkeletonRec;
extern XmSkeletonClassRec xmSkeletonClassRec;
#endif
