---
nav_exclude: true
---

# DataField modify-verify conversion

The DataField modify-verify bridge now treats `wcstombs` and `mbstowcs`
results as `size_t`, as required by the C library contract. Conversion failure
(`(size_t)-1`) and lengths that cannot fit Motif's signed callback lengths are
rejected instead of being tested as negative `int` values or wrapped into a
small length. Invalid insertion lengths are rejected before allocation and
character-width multiplication.

Regression coverage:

```text
python3 tests/automated/check-datafield-modify-verify-conversion.py
```
