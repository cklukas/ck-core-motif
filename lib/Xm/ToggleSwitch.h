/* Additive, compact binary switch widget. */
#ifndef _XmToggleSwitch_h
#define _XmToggleSwitch_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmToggleSwitchWidgetClass;
typedef struct _XmToggleSwitchClassRec *XmToggleSwitchWidgetClass;
typedef struct _XmToggleSwitchRec *XmToggleSwitchWidget;

#ifndef XmIsToggleSwitch
#define XmIsToggleSwitch(w) XtIsSubclass((w), xmToggleSwitchWidgetClass)
#endif

typedef struct {
    int reason;                 /* XmCR_VALUE_CHANGED */
    XEvent *event;              /* NULL for resource/API changes */
    Boolean set;                /* new state */
} XmToggleSwitchCallbackStruct;

Widget XmCreateToggleSwitch(Widget parent, char *name, ArgList args,
                            Cardinal arg_count);
Widget XmVaCreateToggleSwitch(Widget parent, char *name, ...);
Widget XmVaCreateManagedToggleSwitch(Widget parent, char *name, ...);
Boolean XmToggleSwitchGetState(Widget widget);
void XmToggleSwitchSetState(Widget widget, Boolean state, Boolean notify);

#ifdef __cplusplus
}
#endif

#endif /* _XmToggleSwitch_h */
