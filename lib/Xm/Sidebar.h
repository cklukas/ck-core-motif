/* Additive bounded sidebar; navigation policy remains application-owned. */
#ifndef _XmSidebar_h
#define _XmSidebar_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmSidebarWidgetClass;
typedef struct _XmSidebarClassRec *XmSidebarWidgetClass;
typedef struct _XmSidebarRec *XmSidebarWidget;

#ifndef XmIsSidebar
#define XmIsSidebar(w) XtIsSubclass((w), xmSidebarWidgetClass)
#endif

typedef struct {
    int reason;
    XEvent *event;
    Boolean expanded;
} XmSidebarCallbackStruct;

enum { XmSIDEBAR_REASON_TOGGLED = 1 };

Widget XmCreateSidebar(Widget parent, char *name, ArgList args, Cardinal count);
Widget XmVaCreateSidebar(Widget parent, char *name, ...);
Widget XmVaCreateManagedSidebar(Widget parent, char *name, ...);
Widget XmSidebarGetContent(Widget widget);
void XmSidebarSetExpanded(Widget widget, Boolean expanded);
Boolean XmSidebarGetExpanded(Widget widget);
void XmSidebarToggle(Widget widget, XEvent *event);

#ifdef __cplusplus
}
#endif

#endif /* _XmSidebar_h */
