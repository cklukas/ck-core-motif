/* Additive delayed, non-modal teaching-tip presentation. */
#ifndef _XmTeachingTip_h
#define _XmTeachingTip_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmTeachingTipWidgetClass;
typedef struct _XmTeachingTipClassRec *XmTeachingTipWidgetClass;
typedef struct _XmTeachingTipRec *XmTeachingTipWidget;

#ifndef XmIsTeachingTip
#define XmIsTeachingTip(w) XtIsSubclass((w), xmTeachingTipWidgetClass)
#endif

Widget XmCreateTeachingTip(Widget parent, char *name, ArgList args,
                            Cardinal arg_count);
void XmTeachingTipShow(Widget tip);
void XmTeachingTipHide(Widget tip);
Boolean XmTeachingTipIsVisible(Widget tip);

#ifdef __cplusplus
}
#endif

#endif /* _XmTeachingTip_h */
