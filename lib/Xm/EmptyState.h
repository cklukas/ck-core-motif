/* Additive presentation composite for empty-content views. */
#ifndef _XmEmptyState_h
#define _XmEmptyState_h
#include <Xm/Xm.h>
#ifdef __cplusplus
extern "C" {
#endif
externalref WidgetClass xmEmptyStateWidgetClass;
typedef struct _XmEmptyStateClassRec *XmEmptyStateWidgetClass;
typedef struct _XmEmptyStateRec *XmEmptyStateWidget;
#ifndef XmIsEmptyState
#define XmIsEmptyState(w) XtIsSubclass((w), xmEmptyStateWidgetClass)
#endif
typedef struct {
    int reason;
    XEvent *event;
} XmEmptyStateCallbackStruct;
Widget XmCreateEmptyState(Widget parent, char *name, ArgList args, Cardinal count);
Widget XmVaCreateEmptyState(Widget parent, char *name, ...);
Widget XmVaCreateManagedEmptyState(Widget parent, char *name, ...);
Widget XmEmptyStateGetMessage(Widget widget);
Widget XmEmptyStateGetAction(Widget widget);
void XmEmptyStateInvokeAction(Widget widget, XEvent *event);
#ifdef __cplusplus
}
#endif
#endif /* _XmEmptyState_h */
