---
nav_exclude: true
---

# Calendar coordinate bounds

Calendar cell and heading positions are calculated in ordinary integers, then
clamped before being passed to `XmStringDraw`'s signed Xt coordinate
parameters.  Wide controls therefore clip safely instead of wrapping text
coordinates.

`tests/automated/check-calendar-coordinate-bounds.py` protects this drawing
boundary.
