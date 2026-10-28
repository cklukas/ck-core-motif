---
nav_exclude: true
---

# TextField wide preedit append allocation

Wide overstrike preedit appends now validate the combined character count and
byte size before `XtRealloc`. This prevents wrapped sums or an oversized
wide-character allocation from narrowing into an invalid Xt `Cardinal`.

Regression check:

```text
python3 tests/automated/check-textfield-preedit-wide-append-allocation.py
```
