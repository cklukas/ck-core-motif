/* Internal pixel-symmetric modern control surfaces. */
#ifndef _XmAppearanceDrawI_h
#define _XmAppearanceDrawI_h
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
extern Boolean _XmAppearanceConnectedScrolledWindow(Widget);
extern Boolean _XmAppearanceDrawScrolledChildFrame(Widget);
extern Boolean _XmAppearanceDrawCascadeIndicator(Widget, int, int, int, int, unsigned char);
extern Boolean _XmAppearanceDrawButtonFocus(Widget, Boolean);
extern Boolean _XmAppearanceDrawFrameRegion(Widget, int, int, int, int, int);
extern void _XmAppearanceFillRoundedRectangle(Display *, Drawable, GC,
    int, int, int, int, int);
extern Region _XmAppearanceRoundedRectangleRegion(int, int, int, int, int);
/* Angles use X's degrees * 64, positive counterclockwise. Thickness is
 * inward from the exact diameter bounds, independent of GC line attributes. */
extern void _XmAppearanceDrawRing(Display *, Drawable, GC,
    int, int, int, int, int, int);
extern void _XmAppearanceRingBounds(int, int, int, int, int,
    int *, int *, int *);
/* Balanced rounded chevron, centered in the supplied rectangle. */
extern void _XmAppearanceDrawChevron(Display *, Drawable, GC,
    int, int, int, int, int, unsigned char);
extern Pixel _XmAppearanceTextOnFill(Widget, Pixel);
extern Boolean _XmAppearanceDrawPickerFrame(Widget, Boolean);
extern Boolean _XmAppearancePickerSize(Widget, XmRenderTable, const char *, Dimension *, Dimension *);
#endif
