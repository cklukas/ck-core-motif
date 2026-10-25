/* Additive per-instance transient notification composite. */
#ifndef _XmToast_h
#define _XmToast_h
#include <Xm/Xm.h>
#ifdef __cplusplus
extern "C" {
#endif
externalref WidgetClass xmToastWidgetClass;
typedef struct _XmToastClassRec *XmToastWidgetClass;
typedef struct _XmToastRec *XmToastWidget;
#ifndef XmIsToast
#define XmIsToast(w) XtIsSubclass((w), xmToastWidgetClass)
#endif
enum { XmTOAST_SEVERITY_INFO, XmTOAST_SEVERITY_WARNING, XmTOAST_SEVERITY_ERROR };
typedef struct { int reason; XEvent *event; unsigned char severity; } XmToastCallbackStruct;
Widget XmCreateToast(Widget parent, char *name, ArgList args, Cardinal count);
Widget XmVaCreateToast(Widget parent, char *name, ...);
Widget XmVaCreateManagedToast(Widget parent, char *name, ...);
void XmToastShow(Widget widget);
void XmToastDismiss(Widget widget, XEvent *event);
void XmToastPause(Widget widget, Boolean paused);
void XmToastInvokeAction(Widget widget, XEvent *event);
Boolean XmToastIsShown(Widget widget);
#ifdef __cplusplus
}
#endif
#endif /* _XmToast_h */
