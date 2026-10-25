---
nav_exclude: true
---
# ColorPicker redisplay guards

`XmColorPicker` now handles missing drawing GCs, failed swatch-GC allocation,
and an unavailable X11 font without issuing invalid Xlib calls. It also clamps
the swatch height when a very short widget would otherwise underflow the
unsigned dimension subtraction.

The redisplay path obtains the font resource from the live GC with
`XGetGCValues(..., GCFont, ...)` before calling `XQueryFont`. A GC context ID
is not a font ID; passing `XGContextFromGC` to `XQueryFont` raised `BadFont` on
servers whose resource identifiers did not happen to overlap. The installed
consumer deliberately requests a redisplay and installs an X error handler so
that this contract is tested against the server, not only by source inspection.

The focused source contract is checked by
`tests/automated/check-color-picker-redisplay-guard.py`.
