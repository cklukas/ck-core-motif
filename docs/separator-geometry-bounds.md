---
nav_exclude: true
---
# Separator geometry bounds

`XmSeparator` derives its default and recalculated spans from twice the
highlight thickness plus a separator-specific extra.  All orientation and
separator-type paths now use widened arithmetic and clamp that span to the
X11 `Dimension` range, preventing large resource values from wrapping during
initialization or `SetValues`.

The focused source regression is
`tests/automated/check-separator-geometry-bounds.py`.
