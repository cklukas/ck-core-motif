/* Internal full-frame painting. Never substitute a Drawable for a Window. */
#ifndef _XmPaintI_h
#define _XmPaintI_h
#include <X11/IntrinsicP.h>
extern void _XmBufferedPaint(Widget, XtExposeProc, XEvent *, Region);
/* 0: direct/uninitialized, 1: pixmap, 2: DBE (internal diagnostics). */
extern int _XmPaintBufferKind(Widget);
extern void _XmPaintInvalidate(Widget);
extern Drawable _XmPaintDrawable(Widget);
#endif
