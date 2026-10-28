---
nav_exclude: true
---

# XPM extension-write status

XPM file output now validates extension arrays, names, line arrays, and line
strings before serialization. `WriteExtensions` returns input and stream
errors to the top-level writer.

Regression: `tests/automated/check-xpm-extension-write-status.py`
