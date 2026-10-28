---
nav_exclude: true
---

# TextField preedit append allocation

Narrow overstrike preedit appends now validate the existing string length and
the appended suffix before calling `XtRealloc`. Both the `size_t` addition and
the Xt `Cardinal` narrowing are checked, so malformed preedit state cannot
wrap an append buffer size.

Regression check:

```text
python3 tests/automated/check-textfield-preedit-append-allocation.py
```
