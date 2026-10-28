---
nav_exclude: true
---

# TextField input-method conversion

Input-method reset now treats `wcstombs` and `mbstowcs` results as `size_t`,
rejects failed or capacity-filling conversions, and only passes a checked
`int` character count to the replacement path. Invalid preedit text is
discarded as an empty insertion instead of being copied with a wrapped length.

Regression coverage:

```text
python3 tests/automated/check-textfield-input-method-conversion.py
```
