---
nav_exclude: true
---

# TextField selection insertion conversion

Selection insertion now preserves `mbstowcs` results as `size_t` in both
locale-text paths. Conversion failure or an unrepresentable wide-character
count fails the replacement instead of relying on a signed cast and a
negative-result check.

Regression coverage:

```text
python3 tests/automated/check-textfield-selection-insert-conversion.py
```
