---
nav_exclude: true
---

# TextField preedit reset allocation

`TextFieldResetIC` now validates the preedit range before allocating its
temporary wide-character buffers. The length and byte-size calculations are
checked for signed-range, `size_t`, and Xt `Cardinal` limits, preventing an
invalid preedit state from wrapping an allocation size.

Regression check:

```text
python3 tests/automated/check-textfield-resetic-allocation.py
```
