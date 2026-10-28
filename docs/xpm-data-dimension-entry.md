---
nav_exclude: true
---

# XPM data dimension entry guard

`XpmCreateDataFromXpmImage` now rejects zero-width or zero-height images before
allocating its header table.  This prevents invalid input from reaching the
header-pointer copy, which assumes at least one pixel row is available.

Regression: `tests/automated/check-xpm-data-dimension-entry.py`
