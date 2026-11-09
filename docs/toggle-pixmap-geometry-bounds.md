---
nav_exclude: true
---
# ToggleButton pixmap redisplay bounds

ToggleButton temporarily uses its active pixmap dimensions as Label text
rectangle geometry while redrawing. The pixmap API supplies unsigned X11
dimensions wider than Motif's legacy rectangle fields. Redisplay now clamps
the temporary origin and dimensions at those boundaries, preventing a large
pixmap from wrapping the label draw geometry. Normal pixmaps retain the same
layout and drawing behavior.

The focused regression is
`tests/automated/check-toggle-pixmap-geometry-bounds.py`.
