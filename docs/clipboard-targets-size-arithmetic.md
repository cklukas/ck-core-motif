---
nav_exclude: true
---

# Clipboard TARGETS size arithmetic

The `TARGETS` conversion now computes its atom-array byte size in `size_t`,
passes the checked value explicitly to Xt allocation, and widens the returned
element count explicitly. This keeps allocation and X11 property-size
arithmetic aligned without changing the selection format.

Regression: `tests/automated/check-clipboard-targets-size-arithmetic.py`
