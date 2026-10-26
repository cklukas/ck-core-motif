---
nav_exclude: true
---

# Image-cache GC creation inputs

`GetGCForPutImage` now rejects a null screen, null source image, or `None`
destination pixmap before consulting the GC cache or deriving a display.  This
keeps failed or incomplete image conversions from reaching `DisplayOfScreen`
and Xlib with invalid state.

The regression check is `tests/automated/check-image-cache-gc-inputs.py`.
