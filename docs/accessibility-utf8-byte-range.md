---
nav_exclude: true
---

# Accessibility UTF-8 byte-range guard

The AT-SPI Unicode boundary scan now rejects UTF-8 strings whose byte length
cannot be represented by ICU's `int32_t` iterator cursor. This prevents large
multibyte text from being truncated before paragraph or word-boundary
calculation.

Regression check:

```text
python3 tests/automated/check-accessibility-utf8-byte-range.py
```
