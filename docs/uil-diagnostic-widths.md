---
nav_exclude: true
---

# UIL diagnostic and numeric widths

The diagnostic text helpers accept the historical signed `int` type while
symbol-table type codes are unsigned. Their call sites now make the API
boundary explicit. Long integer values converted to floating-point expression
results likewise use an explicit conversion, documenting the intentional
precision boundary.

Regression: `tests/automated/check-uil-diagnostic-widths.py`
