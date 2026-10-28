---
nav_exclude: true
---

# XPM one-row pixel serialization

Both array and buffer serializers reject zero dimensions.  Height-one images
with nonzero width follow the regular final-row path, so their pixels are
included in the generated XPM data instead of being silently omitted.

Regression: `tests/automated/check-xpm-one-row-pixels.py`
