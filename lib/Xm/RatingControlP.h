#ifndef _XmRatingControlP_h
#define _XmRatingControlP_h
#include <Xm/RatingControl.h>
#include <Xm/PrimitiveP.h>
typedef struct _XmRatingControlClassPart { XtPointer extension; } XmRatingControlClassPart;
typedef struct _XmRatingControlClassRec { CoreClassPart core_class; XmPrimitiveClassPart primitive_class; XmRatingControlClassPart rating_class; } XmRatingControlClassRec;
typedef struct _XmRatingControlPart { int count, value; Boolean allow_clear, readonly; XtCallbackList value_changed_callback; GC gc; } XmRatingControlPart;
typedef struct _XmRatingControlRec { CorePart core; XmPrimitivePart primitive; XmRatingControlPart rating; } XmRatingControlRec;
extern XmRatingControlClassRec xmRatingControlClassRec;
#endif /* _XmRatingControlP_h */
