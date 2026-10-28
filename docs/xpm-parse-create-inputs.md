---
nav_exclude: true
---

# XPM parse-and-create inputs

The direct XPM parse-and-create path now validates its display, parser state,
image/output targets, and explicit visual, color-callback, and color-symbol
attributes before setup.  This keeps it consistent with the raw-image path and
prevents malformed state from reaching parser allocation or cleanup calls.

Regression: `tests/automated/check-xpm-parse-create-inputs.py`
