---
nav_exclude: true
---

# UIL numeric-to-integer conversion range

Expression evaluation stores integer results in a C `int`, while parsed UIL
integer values use `long` and floating values use `double` or `float`. The
conversion now rejects values outside the destination range and NaN instead
of relying on an overflowing or implementation-defined cast.

Regression: `tests/automated/check-uil-integer-conversion-range.py`
