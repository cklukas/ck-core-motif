---
nav_exclude: true
---

# UIL compound-string length

Compound-string records store their assembled length in an unsigned 16-bit
field. The semantic evaluator now checks the toolkit-reported length before
storing it, preventing an overlong value from wrapping and describing a
different resource size.

Regression: `tests/automated/check-uil-compound-length.py`
