---
nav_exclude: true
---
# Notebook negative-dimension bounds

`XmNotebook` normalizes geometry requests that use negative widths or heights
by moving the origin and taking the dimension's absolute value. The old macro
performed both operations in `int`, making `INT_MIN` undefined and allowing
the resulting `Position` or `Dimension` to wrap.

The normalization now uses signed wide arithmetic and clamps the resulting
origin and dimension to Motif's representable ranges. Valid geometry requests
retain their existing placement and size.

The focused source regression is
`tests/automated/check-notebook-negative-dimension-bounds.py`.
