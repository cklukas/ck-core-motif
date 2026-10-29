---
nav_exclude: true
---
# Scaled-pixmap image data input

`_XmGetScaledPixmap` now rejects an `XImage` whose dimensions are positive but
whose pixel-data pointer is `NULL`.  Such an image is malformed for rendering;
previously the depth-one XYPixmap conversion could invert the missing data
before X11 had a chance to report an error.

The rejection happens before scaling, cache insertion, or pixel conversion and
returns `XmUNSPECIFIED_PIXMAP` through the existing failure path.  Valid image
loading and the public pixmap ABI are unchanged.

The source regression is
`tests/automated/check-image-data-input.py`.
