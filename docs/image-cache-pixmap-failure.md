---
nav_exclude: true
---
# Image-cache pixmap creation failure

Scaled image construction temporarily locks shared image state when it needs
to normalize a cached depth-one image.  If `XCreatePixmap` fails, cleanup must
release that lock even when no replacement image buffer was allocated.  The
failure path now uses the same cleanup helper as other construction failures,
which restores any shared image state, releases cached pixels, frees the cache
record, and always releases the process lock that was acquired for conversion.

`tests/automated/check-image-cache-pixmap-failure.py` keeps this failure path
centralized so future changes cannot reintroduce the conditional unlock.
