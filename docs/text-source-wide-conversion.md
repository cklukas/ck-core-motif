---
nav_exclude: true
---

# Text-source wide conversion

The string-source wide-value initializer now rejects `wcstombs` failure and
capacity-filling output before using the temporary narrow buffer to establish
the source length. Successful output is explicitly terminated first.

Regression coverage:

```text
python3 tests/automated/check-text-source-wide-conversion.py
```
