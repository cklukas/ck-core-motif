---
nav_exclude: true
---
# Image-cache format restoration

After a cached image is uploaded, temporary format conversion is now undone by
restoring the recorded original format directly. The cleanup no longer asserts
that the original format was specifically `XYPixmap`; valid bitmap conversion
behavior is unchanged and unusual metadata no longer aborts the caller.

The regression check is
`tests/automated/check-image-cache-format-restoration.py`.
