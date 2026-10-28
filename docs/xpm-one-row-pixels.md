---
nav_exclude: true
---

# XPM one-row pixel serialization

Both array and buffer serializers now treat only zero-height images as empty.
Height-one images follow the regular final-row path, so their pixels are
included in the generated XPM data instead of being silently omitted.

Regression: `tests/automated/check-xpm-one-row-pixels.py`
