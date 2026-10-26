---
nav_exclude: true
---
# Wide Text replacement allocation

The generic Text replacement path now checks the wide-string-to-multibyte byte
count before calculating the allocation size, then checks its temporary
multibyte buffer before calling `wcstombs()`.  Arithmetic or allocation
failure restores the insertion-point painting and returns without changing the
document.

Regression coverage: `tests/automated/check-text-wide-replacement-allocation.py`.
