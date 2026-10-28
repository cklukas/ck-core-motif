---
nav_exclude: true
---

# XPM transparent-only images

The XPM image scanner now treats a pixel map containing only its transparent
mask color as a valid zero non-mask-color case.  It skips the zero-size
`XColor` allocation and returns success after the transparent color has been
recorded, so allocator behavior for `malloc(0)` cannot reject valid images.

Regression: `tests/automated/check-xpm-transparent-only.py`
