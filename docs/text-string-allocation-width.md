---
nav_exclude: true
---

# Text string allocation widths

`_XmStringSourceGetString` now passes its already range-checked allocation
through an explicit Xt `Cardinal` conversion and uses `size_t` for block-byte
copies and accumulation. The byte conversion helper likewise makes its
positive copy length and temporary allocation widths explicit.

Regression: `tests/automated/check-text-string-allocation-width.py`
