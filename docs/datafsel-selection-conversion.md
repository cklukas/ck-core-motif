---
nav_exclude: true
---

# DataField selection export conversion

Selection export for `XA_STRING`, locale text, and `COMPOUND_TEXT` now keeps
`wcstombs` results in `size_t` and checks the correct `(size_t)-1` failure
sentinel. Invalid wide selections are reported as empty conversion results
instead of being published with a wrapped length.

Regression coverage:

```text
python3 tests/automated/check-datafsel-selection-conversion.py
```
