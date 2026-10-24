#ifndef _XmBreadcrumbBar_h
#define _XmBreadcrumbBar_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmBreadcrumbBarWidgetClass;
typedef struct _XmBreadcrumbBarClassRec *XmBreadcrumbBarWidgetClass;
typedef struct _XmBreadcrumbBarRec *XmBreadcrumbBarWidget;

#ifndef XmIsBreadcrumbBar
#define XmIsBreadcrumbBar(w) XtIsSubclass((w), xmBreadcrumbBarWidgetClass)
#endif

typedef struct {
    const char *label;
    XtPointer id;
} XmBreadcrumbSegment;

typedef struct {
    int reason;
    XEvent *event;
    Cardinal index;
    XtPointer id;
} XmBreadcrumbBarCallbackStruct;

Widget XmCreateBreadcrumbBar(Widget parent, char *name, ArgList args,
                             Cardinal count);
void XmBreadcrumbBarSetSegments(Widget widget,
                                 const XmBreadcrumbSegment *segments,
                                 Cardinal count);
Cardinal XmBreadcrumbBarGetSegmentCount(Widget widget);
int XmBreadcrumbBarGetCurrentIndex(Widget widget);
void XmBreadcrumbBarSetCurrentIndex(Widget widget, int index,
                                    Boolean notify_change);

#ifdef __cplusplus
}
#endif

#endif /* _XmBreadcrumbBar_h */
