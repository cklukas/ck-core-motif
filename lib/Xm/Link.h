/* Additive, application-dispatched link control. */
#ifndef _XmLink_h
#define _XmLink_h
#include <Xm/Xm.h>
#ifdef __cplusplus
extern "C" {
#endif
externalref WidgetClass xmLinkWidgetClass;
typedef struct _XmLinkClassRec *XmLinkWidgetClass;
typedef struct _XmLinkRec *XmLinkWidget;
#ifndef XmIsLink
#define XmIsLink(w) XtIsSubclass((w), xmLinkWidgetClass)
#endif
typedef struct { int reason; XEvent *event; const char *target; } XmLinkCallbackStruct;
enum { XmLINK_REASON_ACTIVATE = 1 };
Widget XmCreateLink(Widget parent, char *name, ArgList args, Cardinal count);
Widget XmVaCreateLink(Widget parent, char *name, ...);
Widget XmVaCreateManagedLink(Widget parent, char *name, ...);
void XmLinkActivate(Widget widget, XEvent *event);
#ifdef __cplusplus
}
#endif
#endif /* _XmLink_h */
