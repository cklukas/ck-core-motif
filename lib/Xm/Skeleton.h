/* Additive bounded loading placeholder presentation. */
#ifndef _XmSkeleton_h
#define _XmSkeleton_h
#include <Xm/Xm.h>
#ifdef __cplusplus
extern "C" {
#endif
externalref WidgetClass xmSkeletonWidgetClass;
typedef struct _XmSkeletonClassRec *XmSkeletonWidgetClass;
typedef struct _XmSkeletonRec *XmSkeletonWidget;
#ifndef XmIsSkeleton
#define XmIsSkeleton(w) XtIsSubclass((w), xmSkeletonWidgetClass)
#endif
Widget XmCreateSkeleton(Widget parent, char *name, ArgList args, Cardinal count);
Widget XmVaCreateSkeleton(Widget parent, char *name, ...);
Widget XmVaCreateManagedSkeleton(Widget parent, char *name, ...);
void XmSkeletonStart(Widget widget);
void XmSkeletonStop(Widget widget);
Boolean XmSkeletonIsAnimating(Widget widget);
#ifdef __cplusplus
}
#endif
#endif /* _XmSkeleton_h */
