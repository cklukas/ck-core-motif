---
nav_exclude: true
---

# XPM buffer format bounds

XPM buffer headers now use bounded `snprintf` formatting with unsigned
dimension specifiers. This removes the prior signed-format mismatch and
rejects an unexpectedly truncated comment or values line.

Regression: `tests/automated/check-xpm-buffer-format-bounds.py`
