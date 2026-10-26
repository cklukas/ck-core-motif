---
nav_exclude: true
---
# Legacy XmPaned margin bounds

The older `XmPaned` widget adds twice the horizontal or vertical margin to a
negotiated child size before temporarily applying the parent geometry.  That
sum now uses widened arithmetic and clamps to the X11 `Dimension` range in
both orientations, preventing a large margin from wrapping the temporary
layout size.

The focused source regression is
`tests/automated/check-xm-paned-margin-bounds.py`.
