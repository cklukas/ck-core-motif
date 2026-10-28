---
nav_exclude: true
---
# TextField primary-selection wide conversion

Primary-selection insertion now bounds the converted length before narrowing it
to `int` and checks the wide conversion buffer before calling `mbstowcs`.
Allocation or size failure safely rejects the replacement instead of
dereferencing a null buffer.

Regression coverage: `tests/automated/check-textfsel-primary-wide-allocation.py`.
The check follows the explicit bounded conversion result used by the current
implementation.
