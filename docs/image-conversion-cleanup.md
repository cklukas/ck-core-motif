---
nav_exclude: true
---
# Depth-1 conversion cleanup

Depth-1 XYPixmap images may temporarily replace their data with a converted
copy before being uploaded.  The non-cached cleanup path now destroys the
`XImage` first (freeing the converted copy), then frees the saved original
buffer directly.  It no longer dereferences the destroyed image or frees the
wrong allocation.

`tests/automated/check-image-conversion-cleanup.py` protects this ownership
sequence.
