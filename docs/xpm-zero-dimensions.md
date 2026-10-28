---
nav_exclude: true
---

# XPM zero-dimension output

Array and buffer serializers now reject zero-width or zero-height images at
their public entry points and before writing pixel rows.  This matches the
file serializer and avoids partial work for malformed XPM output; valid
one-row images remain supported.

Regression: `tests/automated/check-xpm-zero-dimensions.py`
