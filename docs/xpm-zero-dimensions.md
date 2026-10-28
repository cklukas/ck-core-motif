---
nav_exclude: true
---

# XPM zero-dimension output

Array and buffer serializers now reject zero-width or zero-height images before
writing pixel rows.  This matches the file serializer and avoids reporting
success for malformed XPM output; valid one-row images remain supported.

Regression: `tests/automated/check-xpm-zero-dimensions.py`
