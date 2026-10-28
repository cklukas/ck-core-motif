---
nav_exclude: true
---

# TextField action insertion conversion

Keyboard insertion now preserves the `size_t` result from `mbstowcs`, rejects
conversion failure or lengths too large for Motif's signed replacement API,
and reserves space for the terminating NUL in the input-method lookup buffer.

Regression coverage:

```text
python3 tests/automated/check-textfield-action-insertion-conversion.py
```
