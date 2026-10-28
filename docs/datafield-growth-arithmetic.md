---
nav_exclude: true
---
# DataField replacement growth arithmetic

Replacement growth now checks the resulting character count, including the
signed length-field range, wide-character byte multiplication, doubled
capacity, and legacy allocation field limits before reallocating. Overflow or
unrepresentable growth fails the replacement without changing the existing
buffer.

Regression coverage: `tests/automated/check-datafield-growth-arithmetic.py`.
