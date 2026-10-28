---
nav_exclude: true
---

# TextField wide preedit copy

The XIM wide-preedit path now copies exactly the character count supplied by
the input method and appends its own terminator. It no longer relies on
`wcscpy` finding a NUL within an externally supplied payload.

Regression check:

```text
python3 tests/automated/check-textfield-preedit-wide-copy.py
```
