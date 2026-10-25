# Stepper redisplay guards

`XmStepper` now returns safely when its drawing GC or queried X11 font is
unavailable. This prevents a display-resource allocation failure from reaching
font field access or Xlib drawing calls with a null GC.

The focused source contract is checked by
`tests/automated/check-stepper-redisplay-guard.py`.
