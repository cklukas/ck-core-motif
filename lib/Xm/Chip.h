/* Additive compact label with an optional remove affordance. */
#ifndef _XmChip_h
#define _XmChip_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmChipWidgetClass;
typedef struct _XmChipClassRec *XmChipWidgetClass;
typedef struct _XmChipRec *XmChipWidget;

#ifndef XmIsChip
#define XmIsChip(w) XtIsSubclass((w), xmChipWidgetClass)
#endif

typedef struct {
    int reason;      /* XmCR_ACTIVATE or XmCHIP_REASON_REMOVE */
    XEvent *event;   /* NULL for API calls */
} XmChipCallbackStruct;

enum { XmCHIP_REASON_REMOVE = 1001 };

Widget XmCreateChip(Widget parent, char *name, ArgList args,
                    Cardinal arg_count);
Widget XmVaCreateChip(Widget parent, char *name, ...);
Widget XmVaCreateManagedChip(Widget parent, char *name, ...);
void XmChipInvoke(Widget widget, XEvent *event);
void XmChipRemove(Widget widget, XEvent *event);

#ifdef __cplusplus
}
#endif

#endif /* _XmChip_h */
