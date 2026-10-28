---
nav_exclude: true
---

# DataField substring conversion

DataField substring exports now preserve the `size_t` return type of
`wcstombs`/`mbstowcs` and compare against the proper `(size_t)-1` failure
sentinel. Invalid multibyte conversions no longer appear successful because
an unsigned failure value was compared as a negative `int`.

Regression coverage:

```text
python3 tests/automated/check-datafield-substring-conversion.py
```
