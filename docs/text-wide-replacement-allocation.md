---
nav_exclude: true
---
# Wide Text replacement allocation

The generic Text replacement path now checks its temporary multibyte buffer
before calling `wcstombs()`.  Allocation failure restores the insertion-point
painting and returns without changing the document.

Regression coverage: `tests/automated/check-text-wide-replacement-allocation.py`.
