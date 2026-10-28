---
nav_exclude: true
---

# Text substring conversion

The shared Text substring helper now preserves the native `size_t` result from
`mbstowcs`, rejects conversion failure, and only converts the result to the
signed destination offset after bounds checking. Invalid multibyte input now
fails the copy instead of silently advancing by a wrapped or zero length.

Regression coverage:

```text
python3 tests/automated/check-text-substring-conversion.py
```
