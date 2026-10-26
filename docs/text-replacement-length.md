---
nav_exclude: true
---
# Ordinary Text replacement lengths

The generic Text replacement path now checks the `size_t` result of
`strlen()` before storing it in the legacy `int` block length.  Oversized input
is rejected before source state changes and the insertion point is repainted.

Regression coverage: `tests/automated/check-text-replacement-length.py`.
