---
nav_exclude: true
---

# TextField copy conversion

TextField substring APIs now return the actual number of characters produced
by `wcstombs`/`mbstowcs`, rather than leaving requested lengths in place when
the source contains an earlier terminator. The wide-string setter also rejects
capacity-filling conversion output and explicitly terminates valid output
before handing it to the normal setter.

Regression coverage:

```text
python3 tests/automated/check-textfield-copy-conversion.py
```
