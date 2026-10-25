/* Additive opt-in toolbar; applications retain action meaning and policy. */
#ifndef _XmToolbar_h
#define _XmToolbar_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmToolbarWidgetClass;
typedef struct _XmToolbarClassRec *XmToolbarWidgetClass;
typedef struct _XmToolbarRec *XmToolbarWidget;

#ifndef XmIsToolbar
#define XmIsToolbar(w) XtIsSubclass((w), xmToolbarWidgetClass)
#endif

typedef struct {
    const char *id;
    XmString label;
    XmString shortcut;
    Boolean overflowable;
    unsigned char priority;
    XtPointer user_data;
} XmToolbarAction;

typedef struct {
    int reason;
    XEvent *event;
    const char *id;
    XtPointer user_data;
} XmToolbarCallbackStruct;

enum { XmTOOLBAR_REASON_INVOKED = 1 };

Widget XmCreateToolbar(Widget parent, char *name, ArgList args, Cardinal count);
Widget XmVaCreateToolbar(Widget parent, char *name, ...);
Widget XmVaCreateManagedToolbar(Widget parent, char *name, ...);
void XmToolbarSetActions(Widget widget, const XmToolbarAction *actions,
                         Cardinal count);
Cardinal XmToolbarGetActionCount(Widget widget);
Cardinal XmToolbarGetVisibleActionCount(Widget widget);
Cardinal XmToolbarGetOverflowCount(Widget widget);
void XmToolbarInvoke(Widget widget, Cardinal index, XEvent *event);

#ifdef __cplusplus
}
#endif

#endif /* _XmToolbar_h */
