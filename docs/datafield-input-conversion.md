---
nav_exclude: true
---

# DataField keyboard conversion

DataField keyboard insertion now checks `mbstowcs` failure for both individual
key input and action-string insertion. Invalid multibyte input is rejected
without passing a negative character count to replacement, and the insertion
point is restored before returning.

Regression coverage:

```text
python3 tests/automated/check-datafield-input-conversion.py
```
