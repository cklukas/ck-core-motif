---
nav_exclude: true
---

# XPM buffer color-token width

The XPM buffer serializer now rejects a zero `cpp` value and any color table
entry whose primary token is missing or shorter than the declared token width.
Pixel rows use the same validation and copy exactly the declared number of
bytes, avoiding `strncpy` padding from malformed input.

Regression: `tests/automated/check-xpm-buffer-color-token-width.py`
