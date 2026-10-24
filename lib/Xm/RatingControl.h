#ifndef _XmRatingControl_h
#define _XmRatingControl_h
#include <Xm/Xm.h>
#ifdef __cplusplus
extern "C" {
#endif
externalref WidgetClass xmRatingControlWidgetClass;
typedef struct _XmRatingControlClassRec *XmRatingControlWidgetClass;
typedef struct _XmRatingControlRec *XmRatingControlWidget;
#ifndef XmIsRatingControl
#define XmIsRatingControl(w) XtIsSubclass((w), xmRatingControlWidgetClass)
#endif
typedef struct { int reason; XEvent *event; int count, value; } XmRatingControlCallbackStruct;
Widget XmCreateRatingControl(Widget parent, char *name, ArgList args, Cardinal count);
int XmRatingControlGetValue(Widget widget);
void XmRatingControlSetValue(Widget widget, int value, Boolean notify);
#ifdef __cplusplus
}
#endif
#endif /* _XmRatingControl_h */
