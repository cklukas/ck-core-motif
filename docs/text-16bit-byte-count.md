---
nav_exclude: true
---
# Text 16-bit conversion counts

The 16-bit `_XmTextCharactersToBytes` path now stops before its legacy
integer byte count can wrap when a character emits multiple bytes. The
conversion returns the largest representable count instead of corrupting the
result length.

Regression coverage: `tests/automated/check-text-16bit-byte-count.py`.
