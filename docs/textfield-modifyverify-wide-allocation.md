---
nav_exclude: true
---

# TextField modify-verify wide allocation

The TextField modify-verify bridge now checks callback-provided wide-character
lengths before calculating temporary buffer sizes. Oversized lengths fail via
the existing allocation-failure path instead of wrapping `(length + 1)` or the
wide-character byte multiplication.

Regression check:

```text
python3 tests/automated/check-textfield-modifyverify-wide-allocation.py
```
