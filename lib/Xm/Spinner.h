/* Additive Xt-timer spinner control. */
#ifndef _XmSpinner_h
#define _XmSpinner_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmSpinnerWidgetClass;
typedef struct _XmSpinnerClassRec *XmSpinnerWidgetClass;
typedef struct _XmSpinnerRec *XmSpinnerWidget;

#ifndef XmIsSpinner
#define XmIsSpinner(w) XtIsSubclass((w), xmSpinnerWidgetClass)
#endif

typedef struct {
    int reason; /* XmCR_VALUE_CHANGED */
    XEvent *event;
    Boolean active;
    unsigned int phase;
} XmSpinnerCallbackStruct;

Widget XmCreateSpinner(Widget parent, char *name, ArgList args,
                       Cardinal arg_count);
Widget XmVaCreateSpinner(Widget parent, char *name, ...);
Widget XmVaCreateManagedSpinner(Widget parent, char *name, ...);
Boolean XmSpinnerGetActive(Widget widget);
void XmSpinnerSetActive(Widget widget, Boolean active, Boolean notify);
unsigned int XmSpinnerGetPhase(Widget widget);

#ifdef __cplusplus
}
#endif

#endif /* _XmSpinner_h */
