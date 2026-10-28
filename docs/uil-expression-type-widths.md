---
nav_exclude: true
---

# UIL expression type widths

Expression type codes are stored in compact character fields, and forward
reference flags are stored in an unsigned mask. The evaluator now makes those
legacy-width boundaries explicit when receiving integer helper results and
when clearing a flag, preserving the existing bounded type-code values without
signedness drift.

Regression: `tests/automated/check-uil-expression-type-widths.py`
