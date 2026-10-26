---
nav_exclude: true
---
# ColorPicker redisplay guards

`XmColorPicker` handles missing drawing GCs and failed swatch-GC allocation
without issuing invalid Xlib calls. It also clamps the swatch height when a
very short widget would otherwise underflow the unsigned dimension subtraction.
The value label is centered from `XmStringExtent` metrics, so larger and
complex-script fonts use their actual rendered height.

The focused source contract is checked by
`tests/automated/check-color-picker-redisplay-guard.py`.
