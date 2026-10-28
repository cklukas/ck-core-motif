---
nav_exclude: true
---

# XPM data serialization failure cleanup

`XpmCreateDataFromXpmImage` now owns its final data block until serialization
has succeeded.  Pixel or extension failures release that block through the
common exit path; successful calls transfer it to `data_return` and clear the
local owner so it is not freed twice.

Regression: `tests/automated/check-xpm-data-failure-cleanup.py`
