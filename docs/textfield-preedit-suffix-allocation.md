---
nav_exclude: true
---

# TextField overstrike preedit suffix allocation

The overstrike preedit path now validates its suffix range before allocating
the preserved text. Negative position differences and counts that exceed Xt's
`Cardinal` or `size_t` limits are rejected before either narrow- or
wide-character buffer arithmetic is evaluated.

Regression check:

```text
python3 tests/automated/check-textfield-preedit-suffix-allocation.py
```
