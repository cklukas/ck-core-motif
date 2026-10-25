---
nav_exclude: true
---
# Rejected image cleanup

When a decoded image has invalid dimensions, an unusable scaling ratio, or a
depth mismatch, `_XmGetScaledPixmap` can reject it before creating a pixmap.
Those paths now destroy non-cached `XImage` objects and release temporary XPM
color allocations.  The same release helper is used when a cached pixmap is
finally disposed, keeping color ownership symmetric.

`tests/automated/check-image-rejection-cleanup.py` protects these paths.
