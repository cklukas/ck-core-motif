---
nav_exclude: true
---

# UIL diagnostic width boundaries

Diagnostic source filtering receives an `int` length and status callbacks use
an `int` countdown, while the public command structure stores the delay as an
unsigned value. The implementation now makes the source-length conversion
explicit and clamps an oversized delay to `INT_MAX`. The source-position EOF
sentinel is also compared at its unsigned storage width.

Regression: `tests/automated/check-uil-diagnostic-widths.py`
