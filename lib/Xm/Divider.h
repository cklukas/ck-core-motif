/* Additive text divider; XmSeparator remains unchanged. */
#ifndef _XmDivider_h
#define _XmDivider_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmDividerWidgetClass;
typedef struct _XmDividerClassRec *XmDividerWidgetClass;
typedef struct _XmDividerRec *XmDividerWidget;

#ifndef XmIsDivider
#define XmIsDivider(w) XtIsSubclass((w), xmDividerWidgetClass)
#endif

Widget XmCreateDivider(Widget parent, char *name, ArgList args,
                       Cardinal arg_count);
Widget XmVaCreateDivider(Widget parent, char *name, ...);
Widget XmVaCreateManagedDivider(Widget parent, char *name, ...);

#ifdef __cplusplus
}
#endif

#endif /* _XmDivider_h */
