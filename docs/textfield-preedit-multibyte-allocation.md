---
nav_exclude: true
---

# TextField multibyte preedit allocation

The XIM preedit path now sizes narrow buffers from the multibyte string's
actual byte length. It no longer treats the advertised character count as a
byte count before copying with `strcpy`, preventing overflow for multibyte
locales while retaining the existing callback behavior.

Regression check:

```text
python3 tests/automated/check-textfield-preedit-multibyte-allocation.py
```
