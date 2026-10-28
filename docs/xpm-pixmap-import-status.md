---
nav_exclude: true
---

# XPM pixmap-import status

The internal pixmap-to-`XImage` helper now reports failed geometry queries and
failed `XGetImage` calls.  The data, buffer, file, and direct `XpmImage`
conversion paths propagate that `XpmFileInvalid` result and release any image
already acquired from a paired pixmap or mask.  Invalid server pixmaps
therefore cannot continue as null images into the conversion code.

Regression: `tests/automated/check-xpm-pixmap-import-status.py`
