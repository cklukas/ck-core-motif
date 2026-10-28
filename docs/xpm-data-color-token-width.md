---
nav_exclude: true
---

# XPM data color-token width

The `XpmImage`-to-data serializer now rejects zero `cpp` and missing or short
primary color tokens in both the color table and pixel rows. Valid tokens are
copied exactly, avoiding `strncpy` padding from malformed input.

Regression: `tests/automated/check-xpm-data-color-token-width.py`
