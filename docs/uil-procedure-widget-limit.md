---
nav_exclude: true
---

# UIL procedure widget-type limit

Procedure symbol records retain the widget-type sentinel in an 8-bit field.
Procedure creation now rejects a database object limit that cannot represent
`uil_max_object + 1`, instead of silently wrapping the metadata value.

Regression: `tests/automated/check-uil-procedure-widget-limit.py`
