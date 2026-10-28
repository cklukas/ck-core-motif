---
nav_exclude: true
---

# XPM buffer zero-`cpp` guard

XPM buffer serialization now rejects an image with zero characters per pixel
before checking `width * cpp`, preventing malformed metadata from reaching a
divide-by-zero operation.

Regression: `tests/automated/check-xpm-buffer-cpp-guard.py`
