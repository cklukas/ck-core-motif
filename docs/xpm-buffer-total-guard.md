---
nav_exclude: true
---

# XPM buffer total guard

XPM buffer serialization now checks the header, extension, and comment sizes
before adding them, preventing a wrapped total from reaching allocation and
copy operations.

Regression: `tests/automated/check-xpm-buffer-total-guard.py`
