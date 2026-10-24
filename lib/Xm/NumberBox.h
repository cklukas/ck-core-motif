/* Additive bounded integer entry/spin control. */
#ifndef _XmNumberBox_h
#define _XmNumberBox_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmNumberBoxWidgetClass;
typedef struct _XmNumberBoxClassRec *XmNumberBoxWidgetClass;
typedef struct _XmNumberBoxRec *XmNumberBoxWidget;

#ifndef XmIsNumberBox
#define XmIsNumberBox(w) XtIsSubclass((w), xmNumberBoxWidgetClass)
#endif

typedef struct {
    int reason; /* XmCR_VALUE_CHANGED */
    XEvent *event;
    int minimum, maximum, value, step, page_step;
} XmNumberBoxCallbackStruct;
typedef struct {
    int reason; /* XmCR_VALUE_CHANGED; callback identity distinguishes input errors */
    XEvent *event;
    const char *text;
} XmNumberBoxInvalidCallbackStruct;

Widget XmCreateNumberBox(Widget parent, char *name, ArgList args,
                         Cardinal arg_count);
Widget XmVaCreateNumberBox(Widget parent, char *name, ...);
Widget XmVaCreateManagedNumberBox(Widget parent, char *name, ...);
void XmNumberBoxGetRange(Widget widget, int *minimum, int *maximum);
int XmNumberBoxGetValue(Widget widget);
void XmNumberBoxSetRange(Widget widget, int minimum, int maximum,
                         Boolean notify);
void XmNumberBoxSetValue(Widget widget, int value, Boolean notify);
void XmNumberBoxStep(Widget widget, int direction, Boolean page,
                     Boolean notify);
Boolean XmNumberBoxIsInvalid(Widget widget);

#ifdef __cplusplus
}
#endif

#endif /* _XmNumberBox_h */
