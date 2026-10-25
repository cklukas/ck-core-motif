/* Internal XLibre RandR output-DPI contract helpers. */
#ifndef _XmXlibreDpiI_h
#define _XmXlibreDpiI_h

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/Xrandr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* XLibre defines 96 DPI as the unscaled output baseline. */
#define _XM_XLIBRE_DPI_BASE 96.0
#define _XM_XLIBRE_DPI_MIN 1L
#define _XM_XLIBRE_DPI_MAX 4096L

Boolean _XmXlibreDpiParseProperty(Atom actual_type,
                                  int actual_format,
                                  unsigned long item_count,
                                  unsigned long bytes_after,
                                  long property_value,
                                  double *dpi_return);
/* Decode an Xlib format-32 property only after validating its metadata. */
Boolean _XmXlibreDpiDecodeProperty(Atom actual_type,
                                   int actual_format,
                                   unsigned long item_count,
                                   unsigned long bytes_after,
                                   const long *property_data,
                                   double *dpi_return);

Boolean _XmXlibreDpiServerSupported(Display *display);
Boolean _XmXlibreDpiVendorSupported(const char *vendor);
Boolean _XmXlibreDpiGetOutputDpi(Display *display,
                                 RROutput output,
                                 double *dpi_return);

#ifdef __cplusplus
}
#endif

#endif /* _XmXlibreDpiI_h */
