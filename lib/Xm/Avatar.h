/* Additive non-interactive avatar with pixmap/initials fallback. */
#ifndef _XmAvatar_h
#define _XmAvatar_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmAvatarWidgetClass;
typedef struct _XmAvatarClassRec *XmAvatarWidgetClass;
typedef struct _XmAvatarRec *XmAvatarWidget;

#ifndef XmIsAvatar
#define XmIsAvatar(w) XtIsSubclass((w), xmAvatarWidgetClass)
#endif

Widget XmCreateAvatar(Widget parent, char *name, ArgList args,
                      Cardinal arg_count);
Widget XmVaCreateAvatar(Widget parent, char *name, ...);
Widget XmVaCreateManagedAvatar(Widget parent, char *name, ...);

#ifdef __cplusplus
}
#endif

#endif /* _XmAvatar_h */
