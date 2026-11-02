---
nav_exclude: true
---
# Label resize bounds

Label natural geometry, accelerator margins, and text/accelerator drawing
coordinates are now calculated in wide types and clamped explicitly at the
legacy `Dimension` and `Position` boundaries. Large compound strings, fonts,
or margins no longer silently wrap a Label's requested size or drawing origin.

The focused regression is `tests/automated/check-label-resize-bounds.py`.
