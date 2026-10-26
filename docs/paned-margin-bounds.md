---
nav_exclude: true
---
# Paned margin geometry bounds

Changing an `XmPanedWindow` horizontal margin adjusts the container width by
the difference between the old and new doubled margins.  That delta is now
computed in signed wide arithmetic and clamped to the X11 `Dimension` range
before assignment.  Large increases no longer wrap to a small width, and
large decreases no longer wrap to a huge width.

The focused source regression is
`tests/automated/check-paned-margin-bounds.py`.
