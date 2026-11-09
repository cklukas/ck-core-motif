---
nav_exclude: true
---

# ToggleSwitch coordinate bounds

ToggleSwitch clamps clip origins and extents before narrowing them to X11's
signed/unsigned 16-bit `XRectangle` fields.  Large switch spacing or font
metrics therefore cannot wrap the label or clipping region to an unrelated
screen coordinate.  Its rounded drawing helper also performs its size
conversions only after proving they fit the appearance API's signed geometry
parameters.  The knob is capped at the track height before parity adjustment,
so it remains inside the rounded track at every display scale.

`tests/automated/check-toggle-switch-coordinate-bounds.py` protects this
drawing contract.
