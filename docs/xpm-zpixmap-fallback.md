---
nav_exclude: true
---

# XPM ZPixmap fallback dispatch

The optimized scanner handles 8-, 16-, and 32-bit ZPixmap images directly.
Other valid widths now explicitly use the general `GetImagePixels` reader,
which preserves pixel scanning instead of accidentally retaining a prior status
value and returning an empty result.

Regression: `tests/automated/check-xpm-zpixmap-fallback.py`
