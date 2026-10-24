#ifndef _XmInfoBar_h
#define _XmInfoBar_h
#include <Xm/Xm.h>
#ifdef __cplusplus
extern "C" {
#endif
externalref WidgetClass xmInfoBarWidgetClass;
typedef struct _XmInfoBarClassRec *XmInfoBarWidgetClass;
typedef struct _XmInfoBarRec *XmInfoBarWidget;
#ifndef XmIsInfoBar
#define XmIsInfoBar(w) XtIsSubclass((w), xmInfoBarWidgetClass)
#endif
enum { XmINFO_SEVERITY_INFO, XmINFO_SEVERITY_WARNING, XmINFO_SEVERITY_ERROR };
typedef struct { int reason; XEvent *event; unsigned char severity; } XmInfoBarCallbackStruct;
Widget XmCreateInfoBar(Widget parent, char *name, ArgList args, Cardinal count);
void XmInfoBarDismiss(Widget widget);
void XmInfoBarInvokeAction(Widget widget);
#ifdef __cplusplus
}
#endif
#endif /* _XmInfoBar_h */
