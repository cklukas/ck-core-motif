---
nav_exclude: true
---

# XPM reader close status

The file-to-image and file-to-`XpmImage` readers now propagate a failed
`fclose()` as `XpmOpenFailed` when parsing itself succeeded. Existing parse
errors remain the primary result, while standard input remains unclosed.

Regression: `tests/automated/check-xpm-reader-close-status.py`
