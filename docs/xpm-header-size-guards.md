---
nav_exclude: true
---

# XPM header and color size guards

XPM data serialization now checks accumulated header and color-line sizes
before adding each new line, freeing the just-created line on overflow instead
of publishing a wrapped total.

Regression: `tests/automated/check-xpm-header-size-guards.py`
