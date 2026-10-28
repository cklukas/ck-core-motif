---
nav_exclude: true
---

# XPM data format bounds

XPM data serialization now uses checked `snprintf` calls for dimensions and
hotspots, with unsigned dimension formats and an explicit extension-literal
capacity check.

Regression: `tests/automated/check-xpm-data-format-bounds.py`
