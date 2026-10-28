---
nav_exclude: true
---

# TextField overstrike wide-buffer limit

The overstrike preedit setup now checks the wide suffix size against both
`size_t` and Xt's `Cardinal` limit before allocating the saved text. This
prevents a valid-looking position range from narrowing to an undersized Xt
allocation on platforms where `Cardinal` is smaller than `size_t`.

Regression check:

```text
python3 tests/automated/check-textfield-overstrike-wide-limit.py
```
