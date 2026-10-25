---
nav_exclude: true
---
# PixConv pixmap metadata guard

`_XmGetPixmapBasedGC` now configures a tile or stipple GC only when
`XmeGetPixmapData` successfully resolves the pixmap metadata.  A lookup failure
leaves the existing foreground/background GC in place, avoiding an
uninitialized depth decision and an invalid pixmap resource in the GC.

The focused regression is
`tests/automated/check-pixconv-pixmap-metadata.py`.
