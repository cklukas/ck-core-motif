/* Bounded status presentation; applications own the status model and policy. */
#ifndef _XmStatusBar_h
#define _XmStatusBar_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmStatusBarWidgetClass;
typedef struct _XmStatusBarClassRec *XmStatusBarWidgetClass;
typedef struct _XmStatusBarRec *XmStatusBarWidget;

#ifndef XmIsStatusBar
#define XmIsStatusBar(w) XtIsSubclass((w), xmStatusBarWidgetClass)
#endif

enum {
    XmSTATUS_BAR_ITEM_TEXT = 0,
    XmSTATUS_BAR_ITEM_PROGRESS = 1
};

typedef struct {
    const char *id;
    XmString label;
    unsigned char kind;
    unsigned char priority;
    int value;
    int maximum;
    XtPointer user_data;
} XmStatusBarItem;

Widget XmCreateStatusBar(Widget parent, char *name, ArgList args,
                         Cardinal count);
Widget XmVaCreateStatusBar(Widget parent, char *name, ...);
Widget XmVaCreateManagedStatusBar(Widget parent, char *name, ...);
void XmStatusBarSetItems(Widget widget, const XmStatusBarItem *items,
                         Cardinal count);
Cardinal XmStatusBarGetItemCount(Widget widget);
Cardinal XmStatusBarGetVisibleItemCount(Widget widget);
Cardinal XmStatusBarGetOverflowCount(Widget widget);

#ifdef __cplusplus
}
#endif

#endif /* _XmStatusBar_h */
