/* Additive search field preserving ordinary XmTextField editing semantics. */
#ifndef _XmSearchBox_h
#define _XmSearchBox_h
#include <Xm/Xm.h>
#ifdef __cplusplus
extern "C" {
#endif
externalref WidgetClass xmSearchBoxWidgetClass;
typedef struct _XmSearchBoxClassRec *XmSearchBoxWidgetClass;
typedef struct _XmSearchBoxRec *XmSearchBoxWidget;
#ifndef XmIsSearchBox
#define XmIsSearchBox(w) XtIsSubclass((w), xmSearchBoxWidgetClass)
#endif
typedef struct { int reason; XEvent *event; const char *query; } XmSearchBoxCallbackStruct;
enum { XmSEARCH_REASON_QUERY_CHANGED = 1, XmSEARCH_REASON_COMMITTED = 2, XmSEARCH_REASON_CLEARED = 3 };
Widget XmCreateSearchBox(Widget parent, char *name, ArgList args, Cardinal count);
Widget XmVaCreateSearchBox(Widget parent, char *name, ...);
Widget XmVaCreateManagedSearchBox(Widget parent, char *name, ...);
char *XmSearchBoxGetString(Widget widget);
void XmSearchBoxSetString(Widget widget, const char *value);
void XmSearchBoxClear(Widget widget);
void XmSearchBoxCommit(Widget widget, XEvent *event);
#ifdef __cplusplus
}
#endif
#endif /* _XmSearchBox_h */
