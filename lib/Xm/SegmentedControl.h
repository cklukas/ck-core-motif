/* Additive mutually-exclusive segmented picker. */
#ifndef _XmSegmentedControl_h
#define _XmSegmentedControl_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmSegmentedControlWidgetClass;
typedef struct _XmSegmentedControlClassRec *XmSegmentedControlWidgetClass;
typedef struct _XmSegmentedControlRec *XmSegmentedControlWidget;

#ifndef XmIsSegmentedControl
#define XmIsSegmentedControl(w) XtIsSubclass((w), xmSegmentedControlWidgetClass)
#endif

enum { XmSEGMENTED_OVERFLOW_CLIP = 0 };

typedef struct {
    const char *id;       /* copied by XmSegmentedControlSetItems */
    XmString label;       /* copied by XmSegmentedControlSetItems */
    Pixmap pixmap;        /* XmUNSPECIFIED_PIXMAP means no icon */
    Boolean enabled;
} XmSegmentedControlItem;

typedef struct {
    int reason;            /* XmCR_VALUE_CHANGED */
    XEvent *event;         /* NULL for resource/API changes */
    int selected_index;    /* -1 when the selection was cleared */
    const char *selected_id; /* borrowed until the next mutation */
} XmSegmentedControlCallbackStruct;

Widget XmCreateSegmentedControl(Widget parent, char *name, ArgList args,
                                Cardinal arg_count);
Widget XmVaCreateSegmentedControl(Widget parent, char *name, ...);
Widget XmVaCreateManagedSegmentedControl(Widget parent, char *name, ...);
Boolean XmSegmentedControlSetItems(Widget widget,
                                   const XmSegmentedControlItem *items,
                                   Cardinal count);
Cardinal XmSegmentedControlGetItemCount(Widget widget);
int XmSegmentedControlGetSelectedIndex(Widget widget);
const char *XmSegmentedControlGetItemId(Widget widget, Cardinal index);
Boolean XmSegmentedControlSetSelectedIndex(Widget widget, int index,
                                           Boolean notify);
void XmSegmentedControlClearSelection(Widget widget, Boolean notify);

#ifdef __cplusplus
}
#endif

#endif /* _XmSegmentedControl_h */
