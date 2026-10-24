/* Additive compact bounded integer stepper. */
#ifndef _XmStepper_h
#define _XmStepper_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmStepperWidgetClass;
typedef struct _XmStepperClassRec *XmStepperWidgetClass;
typedef struct _XmStepperRec *XmStepperWidget;
#ifndef XmIsStepper
#define XmIsStepper(w) XtIsSubclass((w), xmStepperWidgetClass)
#endif
typedef struct {
    int reason; /* XmCR_VALUE_CHANGED */
    XEvent *event;
    int minimum, maximum, value, step;
} XmStepperCallbackStruct;
Widget XmCreateStepper(Widget parent, char *name, ArgList args, Cardinal count);
Widget XmVaCreateStepper(Widget parent, char *name, ...);
Widget XmVaCreateManagedStepper(Widget parent, char *name, ...);
void XmStepperGetRange(Widget widget, int *minimum, int *maximum);
int XmStepperGetValue(Widget widget);
void XmStepperSetRange(Widget widget, int minimum, int maximum, Boolean notify);
void XmStepperSetValue(Widget widget, int value, Boolean notify);
void XmStepperStep(Widget widget, int direction, Boolean notify);
#ifdef __cplusplus
}
#endif
#endif /* _XmStepper_h */
