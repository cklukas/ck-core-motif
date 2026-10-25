---
nav_exclude: true
---
# ColorPicker redisplay guards

`XmColorPicker` now handles missing drawing GCs, failed swatch-GC allocation,
and an unavailable X11 font without issuing invalid Xlib calls. It also clamps
the swatch height when a very short widget would otherwise underflow the
unsigned dimension subtraction.

The focused source contract is checked by
`tests/automated/check-color-picker-redisplay-guard.py`.
