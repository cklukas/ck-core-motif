---
nav_exclude: true
---

# XPM buffer format bounds

XPM buffer headers now use bounded `snprintf` formatting with unsigned
dimension specifiers. This removes the prior signed-format mismatch and
rejects an unexpectedly truncated comment or values line. The serializer also
checks the remaining capacity before writing its final `};` terminator, so a
malformed or internally inconsistent size calculation fails cleanly instead of
writing past the allocation.

Regressions: `tests/automated/check-xpm-buffer-format-bounds.py` and
`tests/automated/check-xpm-buffer-final-terminator-bounds.py`
