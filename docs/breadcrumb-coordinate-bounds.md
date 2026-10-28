---
nav_exclude: true
---

# BreadcrumbBar coordinate bounds

BreadcrumbBar keeps path-width and hit-test calculations in `int` values, but
now clamps coordinates when handing them to `XmStringDraw`, whose Xt position
arguments are signed 16-bit values.  Very long paths remain safely clipped
instead of wrapping labels to unrelated coordinates.

`tests/automated/check-breadcrumb-coordinate-bounds.py` protects this drawing
boundary.
