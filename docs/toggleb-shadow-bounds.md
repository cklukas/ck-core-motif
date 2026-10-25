---
nav_exclude: true
---
# ToggleButton shadow bounds

The ToggleButton menu redisplay path now checks the widget's inner width and
height and both shadow GCs before drawing.  Collapsed or partially initialized
widgets therefore skip the shadow instead of passing wrapped inset dimensions
to Xlib.

The source-level regression is `tests/automated/check-toggleb-shadow-bounds.py`.
