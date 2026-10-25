---
nav_exclude: true
---
# Xm image-cache output validation

`GetImage` is the shared image-cache reader behind `_XmGetImage` and pixmap
scaling. It now rejects a null image output pointer and clears a valid output
before any cache lookup or filename validation. A failed lookup therefore
cannot leave a caller's previous `XImage *` exposed as though it were current.

Successful cache hits and image loads retain their existing behavior. The
source-level regression is `tests/automated/check-xm-get-image-output.py`.
