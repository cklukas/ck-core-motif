---
nav_exclude: true
---

# XPM color-write status

XPM file output now validates the color table and each primary color token
before dereferencing them. `WriteColors` returns allocation/input/stream status
to the top-level writer instead of silently continuing after malformed input.

Regression: `tests/automated/check-xpm-color-write-status.py`
