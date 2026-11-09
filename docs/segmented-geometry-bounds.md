---
nav_exclude: true
---
# Segmented-control geometry bounds

Segmented-control preferred width now accumulates child widths and spacing in
a wider type and clamps the result to the platform `Dimension` maximum. Large
segment sets no longer wrap into unexpectedly narrow geometry requests;
ordinary layouts are unchanged.

Modern drawing also verifies that the inner rounded face fits after its stroke
inset before passing its dimensions to X11. Very small segments now retain
their outer face instead of narrowing a negative inner width or height through
an unsigned drawing parameter. Focus and separator rectangles use explicit
representable unsigned extents.

`tests/automated/check-segmented-geometry-bounds.py` protects this calculation.
`tests/automated/check-segmented-render-geometry.py` protects the drawing
bounds.
