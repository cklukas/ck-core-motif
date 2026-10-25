/* Additive compact determinate/indeterminate progress ring. */
#ifndef _XmProgressRing_h
#define _XmProgressRing_h

#include <Xm/ProgressBar.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmProgressRingWidgetClass;
typedef struct _XmProgressRingClassRec *XmProgressRingWidgetClass;
typedef struct _XmProgressRingRec *XmProgressRingWidget;

#ifndef XmIsProgressRing
#define XmIsProgressRing(w) XtIsSubclass((w), xmProgressRingWidgetClass)
#endif

Widget XmCreateProgressRing(Widget parent, char *name, ArgList args,
                            Cardinal arg_count);
Widget XmVaCreateProgressRing(Widget parent, char *name, ...);
Widget XmVaCreateManagedProgressRing(Widget parent, char *name, ...);
void XmProgressRingGetRange(Widget widget, int *minimum, int *maximum);
int XmProgressRingGetValue(Widget widget);
void XmProgressRingSetRange(Widget widget, int minimum, int maximum,
                            Boolean notify);
void XmProgressRingSetValue(Widget widget, int value, Boolean notify);
Boolean XmProgressRingGetIndeterminate(Widget widget);
void XmProgressRingSetIndeterminate(Widget widget, Boolean indeterminate,
                                    Boolean notify);

#ifdef __cplusplus
}
#endif

#endif /* _XmProgressRing_h */
