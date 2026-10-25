/* Additive compact status badge. */
#ifndef _XmBadge_h
#define _XmBadge_h
#include <Xm/Xm.h>
#ifdef __cplusplus
extern "C" {
#endif
externalref WidgetClass xmBadgeWidgetClass;
typedef struct _XmBadgeClassRec *XmBadgeWidgetClass;
typedef struct _XmBadgeRec *XmBadgeWidget;
#ifndef XmIsBadge
#define XmIsBadge(w) XtIsSubclass((w), xmBadgeWidgetClass)
#endif
enum {
    XmBADGE_INFO = 0,
    XmBADGE_SUCCESS = 1,
    XmBADGE_WARNING = 2,
    XmBADGE_ERROR = 3
};
Widget XmCreateBadge(Widget parent, char *name, ArgList args, Cardinal count);
Widget XmVaCreateBadge(Widget parent, char *name, ...);
Widget XmVaCreateManagedBadge(Widget parent, char *name, ...);
int XmBadgeGetDisplayedCount(Widget widget);
#ifdef __cplusplus
}
#endif
#endif /* _XmBadge_h */
