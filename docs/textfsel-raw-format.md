---
nav_exclude: true
---
# TextField raw transfer format validation

The TextField raw-target transfer path now rejects non-byte selection formats
before copying or converting the payload. This keeps 16-bit and 32-bit
external data from being reinterpreted as locale bytes.

Regression coverage: `tests/automated/check-textfsel-raw-format.py`.
