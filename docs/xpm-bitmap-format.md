---
nav_exclude: true
---

# XPM bitmap format input

Both XPM image-conversion paths now accept only `XYBitmap` or `ZPixmap` when
`XpmBitmapFormat` is explicitly supplied.  Invalid format values return
`XpmFileInvalid` before X11 image construction; omitted and valid format
settings retain their existing behavior.

Regression: `tests/automated/check-xpm-bitmap-format.py`
