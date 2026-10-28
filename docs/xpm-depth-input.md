---
nav_exclude: true
---

# XPM explicit depth input

Both XPM image-conversion paths now reject an `XpmDepth` attribute with value
zero before passing it to X11 image construction.  Calls that omit the flag
continue to use the display's default depth, and valid explicit depths retain
their existing behavior.

Regression: `tests/automated/check-xpm-depth-input.py`
