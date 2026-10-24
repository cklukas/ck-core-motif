/* Additive non-modal anchored popup primitive. */
#ifndef _XmFlyout_h
#define _XmFlyout_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmFlyoutWidgetClass;
typedef struct _XmFlyoutClassRec *XmFlyoutWidgetClass;
typedef struct _XmFlyoutRec *XmFlyoutWidget;

#ifndef XmIsFlyout
#define XmIsFlyout(w) XtIsSubclass((w), xmFlyoutWidgetClass)
#endif

Widget XmCreateFlyout(Widget parent, char *name, ArgList args,
                      Cardinal arg_count);
void XmFlyoutShow(Widget flyout);
void XmFlyoutHide(Widget flyout);
Boolean XmFlyoutIsVisible(Widget flyout);

#ifdef __cplusplus
}
#endif

#endif /* _XmFlyout_h */
