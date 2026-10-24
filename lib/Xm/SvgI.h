/* Internal optional SVG image-loader contract. */
#ifndef _XmSvgI_h
#define _XmSvgI_h

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Return zero only when an opaque XImage was produced successfully. */
int _XmSvgGetImage(Screen *screen, const char *filename, Pixel background,
                   XImage **image_return);

#ifdef __cplusplus
}
#endif

#endif /* _XmSvgI_h */
