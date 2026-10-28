---
nav_exclude: true
---

# Text gap-span validation

Text replacement now validates the gapped-buffer pointers and ordering before
converting their signed pointer difference to `size_t`. A malformed gap state
is rejected before editing continues, preventing a wrapped span from driving
later pointer arithmetic.

Regression: `tests/automated/check-text-gap-span.py`
