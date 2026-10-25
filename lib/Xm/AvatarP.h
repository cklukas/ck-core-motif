#ifndef _XmAvatarP_h
#define _XmAvatarP_h

#include <Xm/Avatar.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmAvatarClassPart { XtPointer extension; } XmAvatarClassPart;
typedef struct _XmAvatarClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmAvatarClassPart avatar_class;
} XmAvatarClassRec;

typedef struct _XmAvatarPart {
    XmString initials;
    Pixmap pixmap;
    Dimension size;
    GC foreground_gc;
    XFontStruct *fallback_font;
} XmAvatarPart;

typedef struct _XmAvatarRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmAvatarPart avatar;
} XmAvatarRec;

extern XmAvatarClassRec xmAvatarClassRec;

#endif /* _XmAvatarP_h */
