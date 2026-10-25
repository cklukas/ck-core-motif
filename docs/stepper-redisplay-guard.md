# Stepper redisplay guards

`XmStepper` now returns safely when its drawing GC or queried X11 font is
unavailable. This prevents a display-resource allocation failure from reaching
font field access or Xlib drawing calls with a null GC.

The font resource is retrieved from the live GC with `XGetGCValues(...,
GCFont, ...)` before `XQueryFont`; a GC context ID is not a font ID and must
not be passed to the latter.

The focused source contract is checked by
`tests/automated/check-stepper-redisplay-guard.py`.
