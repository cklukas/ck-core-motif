---
nav_exclude: true
---

# UIL expression-frame widths

Parser expression operators and flags are held in compact frame fields. The
semantic-action code now narrows the known operator and flag values explicitly
when constructing binary and unary expression frames, preserving the legacy
representation without implicit signedness or width conversions.

Regression: `tests/automated/check-uil-expression-frame-widths.py`
