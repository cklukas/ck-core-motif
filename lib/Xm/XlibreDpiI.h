/* Internal XLibre RandR output-DPI contract helpers. */
#ifndef _XmXlibreDpiI_h
#define _XmXlibreDpiI_h

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/Xrandr.h>
#include <stddef.h>

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
Boolean _XmXlibreDpiGetOutputDpi(Display *display,
                                 RROutput output,
                                 double *dpi_return);

/* Internal shell-boundary geometry; coordinates are root-window pixels. */
typedef struct {
    int x;
    int y;
    unsigned int width;
    unsigned int height;
} _XmXlibreDpiRect;

typedef struct {
    RROutput output;
    _XmXlibreDpiRect geometry;
} _XmXlibreDpiOutput;

/* Select the center-containing, or nearest, output with stable ID tie-break. */
RROutput _XmXlibreDpiSelectOutput(const _XmXlibreDpiOutput *outputs,
                                  size_t output_count,
                                  const _XmXlibreDpiRect *client_area);

/*
 * A shell-boundary transaction is always delivered on the owning Xt
 * application thread.  The callback runs after one coalesced reflow request;
 * it must update only derived/default metrics and must not retain the shell.
 */
typedef void (*_XmXlibreDpiChangeProc)(Widget shell,
                                       double old_scale,
                                       double new_scale,
                                       RROutput old_output,
                                       RROutput new_output,
                                       XtPointer closure);

Boolean _XmXlibreDpiInstallShell(Widget shell,
                                 _XmXlibreDpiChangeProc change_proc,
                                 XtPointer closure);
void _XmXlibreDpiUninstallShell(Widget shell);
Boolean _XmXlibreDpiGetWidgetScale(Widget widget, double *scale_return);

#ifdef __cplusplus
}
#endif

#endif /* _XmXlibreDpiI_h */
