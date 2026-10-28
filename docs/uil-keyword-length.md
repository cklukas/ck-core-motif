---
nav_exclude: true
---

# UIL keyword-length validation

Keyword lookup receives an unsigned source length while the generated maximum
keyword length remains an `int` for ABI compatibility. Lookup now rejects a
negative configuration and compares the values only after an explicit width
conversion, avoiding signedness-dependent acceptance of oversized names.

Regression: `tests/automated/check-uil-keyword-length.py`
