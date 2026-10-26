---
nav_exclude: true
---
# Column default-size bounds

`XmColumn` derives its default width and height from twice the manager shadow
and margin.  The calculation now uses widened arithmetic and clamps to the
X11 `Dimension` range before assignment, preventing large resource values from
wrapping the initial geometry.

The focused source regression is
`tests/automated/check-column-default-size-bounds.py`.
