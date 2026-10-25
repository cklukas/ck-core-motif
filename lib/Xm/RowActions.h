/* Additive opt-in row-action strip; does not own or modify list rows. */
#ifndef _XmRowActions_h
#define _XmRowActions_h
#include <Xm/Xm.h>
#ifdef __cplusplus
extern "C" {
#endif
externalref WidgetClass xmRowActionsWidgetClass;
typedef struct _XmRowActionsClassRec *XmRowActionsWidgetClass;
typedef struct _XmRowActionsRec *XmRowActionsWidget;
#ifndef XmIsRowActions
#define XmIsRowActions(w) XtIsSubclass((w), xmRowActionsWidgetClass)
#endif
enum { XmROW_ACTIONS_SIDE_START, XmROW_ACTIONS_SIDE_END };
typedef struct { const char *id; XmString label; XmString shortcut; XtPointer user_data; } XmRowActionItem;
typedef struct { int reason; XEvent *event; const char *id; XtPointer user_data; } XmRowActionsCallbackStruct;
enum { XmROW_ACTIONS_REASON_INVOKED = 1 };
Widget XmCreateRowActions(Widget parent, char *name, ArgList args, Cardinal count);
Widget XmVaCreateRowActions(Widget parent, char *name, ...);
Widget XmVaCreateManagedRowActions(Widget parent, char *name, ...);
void XmRowActionsSetItems(Widget widget, const XmRowActionItem *items, Cardinal count);
Cardinal XmRowActionsGetItemCount(Widget widget);
void XmRowActionsReveal(Widget widget, Boolean revealed);
Boolean XmRowActionsIsRevealed(Widget widget);
void XmRowActionsInvoke(Widget widget, Cardinal index, XEvent *event);
#ifdef __cplusplus
}
#endif
#endif /* _XmRowActions_h */
