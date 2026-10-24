/* Additive determinate/indeterminate progress control. */
#ifndef _XmProgressBar_h
#define _XmProgressBar_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmProgressBarWidgetClass;
typedef struct _XmProgressBarClassRec *XmProgressBarWidgetClass;
typedef struct _XmProgressBarRec *XmProgressBarWidget;

#ifndef XmIsProgressBar
#define XmIsProgressBar(w) XtIsSubclass((w), xmProgressBarWidgetClass)
#endif

typedef struct {
    int reason;                 /* XmCR_VALUE_CHANGED */
    XEvent *event;
    int minimum;
    int maximum;
    int value;
    Boolean indeterminate;
} XmProgressBarCallbackStruct;

Widget XmCreateProgressBar(Widget parent, char *name, ArgList args,
                           Cardinal arg_count);
Widget XmVaCreateProgressBar(Widget parent, char *name, ...);
Widget XmVaCreateManagedProgressBar(Widget parent, char *name, ...);
void XmProgressBarGetRange(Widget widget, int *minimum, int *maximum);
int XmProgressBarGetValue(Widget widget);
void XmProgressBarSetRange(Widget widget, int minimum, int maximum,
                           Boolean notify);
void XmProgressBarSetValue(Widget widget, int value, Boolean notify);
Boolean XmProgressBarGetIndeterminate(Widget widget);
void XmProgressBarSetIndeterminate(Widget widget, Boolean indeterminate,
                                   Boolean notify);

#ifdef __cplusplus
}
#endif

#endif /* _XmProgressBar_h */
