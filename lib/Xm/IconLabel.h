/* Additive icon-and-text presentation label. */
#ifndef _XmIconLabel_h
#define _XmIconLabel_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmIconLabelWidgetClass;
typedef struct _XmIconLabelClassRec *XmIconLabelWidgetClass;
typedef struct _XmIconLabelRec *XmIconLabelWidget;

#ifndef XmIsIconLabel
#define XmIsIconLabel(w) XtIsSubclass((w), xmIconLabelWidgetClass)
#endif

Widget XmCreateIconLabel(Widget parent, char *name, ArgList args,
                         Cardinal arg_count);
Widget XmVaCreateIconLabel(Widget parent, char *name, ...);
Widget XmVaCreateManagedIconLabel(Widget parent, char *name, ...);

#ifdef __cplusplus
}
#endif

#endif /* _XmIconLabel_h */
