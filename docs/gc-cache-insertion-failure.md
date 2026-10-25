---
nav_exclude: true
---
# GC cache insertion failure

`GetGCForPutImage` now checks insertion of a newly created GC into the GC
cache. If bucket allocation fails, it unlocks the process cache, frees the X11
GC and its pending entry, and returns failure so the caller can roll back the
pixmap build.

The focused regression is `tests/automated/check-xm-gc-insertion-failure.py`.
