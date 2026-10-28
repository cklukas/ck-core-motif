---
nav_exclude: true
---

# DataField public conversion bounds

The public wide-string setter now terminates only successful conversions and
turns a failed or capacity-filling `wcstombs` result into an empty value
before handing it to the normal setter. Selection export rejects the same
capacity-filling result before indexing its output buffer, returning `NULL`
instead of writing past the allocation.

Regression coverage:

```text
python3 tests/automated/check-datafield-public-conversion-bounds.py
```
