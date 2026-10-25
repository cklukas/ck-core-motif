---
nav_exclude: true
---
# Xm GC call-site rollback

When `GetGCForPutImage` cannot create a GC, `_XmGetScaledPixmap` now removes
the provisional pixmap from both caches, frees the X pixmap, and releases the
temporary image and pixel state. The null GC is never passed to
`_XmPutScaledImage`.

The source-level regression is `tests/automated/check-xm-gc-callsite-rollback.py`.
