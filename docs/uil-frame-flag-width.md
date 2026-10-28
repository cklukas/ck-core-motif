---
nav_exclude: true
---

# UIL parser frame-flag width

Semantic value entries keep object flags in a 32-bit field, while parser
frames expose a 16-bit flag field. The transfers now make that legacy width
boundary explicit without changing the existing flag values.

Regression: `tests/automated/check-uil-frame-flag-width.py`
