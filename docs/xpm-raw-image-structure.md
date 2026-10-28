---
nav_exclude: true
---

# XPM raw-image structure

The raw XPM-image conversion path now rejects zero geometry, zero
characters-per-pixel or color count, and missing color-table or pixel-data
arrays before entering color allocation or image emission.  Valid parsed and
constructed `XpmImage` values retain their existing behavior; malformed
structures return `XpmFileInvalid` instead of reaching a null dereference.

Regression: `tests/automated/check-xpm-raw-image-structure.py`
