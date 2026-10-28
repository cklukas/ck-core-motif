---
nav_exclude: true
---

# XPM buffer comment formatting

Pixel comments in the XPM buffer serializer now check `snprintf` errors and
capacity before advancing the output cursor. This keeps malformed or
unexpectedly long comment data from desynchronizing the subsequent pixel and
extension writes.

Regression: `tests/automated/check-xpm-buffer-comment-format.py`
