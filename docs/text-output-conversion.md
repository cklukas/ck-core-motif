---
nav_exclude: true
---

# Text output conversion

TextOut backward-scanning paths now preserve `mbstowcs` results as `size_t`,
reject conversion failure or an unrepresentable character count, and only
then use the checked `int` count for scanning and measuring the converted
text.

Regression coverage:

```text
python3 tests/automated/check-text-output-conversion.py
```
