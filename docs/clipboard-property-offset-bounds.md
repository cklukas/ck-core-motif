---
nav_exclude: true
---

# Clipboard property offset bounds

`GetWindowProperty` now checks its unsigned property offset against
`LONG_MAX` before passing it to Xlib’s signed `long` offset parameter. This
prevents malformed or oversized property walks from wrapping during the X11
conversion.

Regression: `tests/automated/check-clipboard-property-offset-bounds.py`
