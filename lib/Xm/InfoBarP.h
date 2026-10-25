#ifndef _XmInfoBarP_h
#define _XmInfoBarP_h
#include <Xm/InfoBar.h>
#include <Xm/PrimitiveP.h>
typedef struct _XmInfoBarClassPart { XtPointer extension; } XmInfoBarClassPart;
typedef struct _XmInfoBarClassRec { CoreClassPart core_class; XmPrimitiveClassPart primitive_class; XmInfoBarClassPart info_class; } XmInfoBarClassRec;
typedef struct _XmInfoBarPart { String message, action_label; unsigned char severity; Boolean closable; XtCallbackList dismiss_callback, action_callback; GC gc; XFontStruct *font; } XmInfoBarPart;
typedef struct _XmInfoBarRec { CorePart core; XmPrimitivePart primitive; XmInfoBarPart info; } XmInfoBarRec;
extern XmInfoBarClassRec xmInfoBarClassRec;
#endif /* _XmInfoBarP_h */
