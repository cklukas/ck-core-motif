---
nav_exclude: true
---

# UIL cycle-mask validation

The semantic validator records cycle state in a bit mask. The already-checked
path now tests whether the mask is nonzero, rather than comparing it with the
literal value `1`; this preserves cycle detection when the mask bit is not the
lowest bit.

Regression: `tests/automated/check-uil-cycle-mask.py`
