---
nav_exclude: true
---
# TextField selection conversion lengths

The TextField selection callback now bounds external lengths before its legacy
integer and wide-character conversions, and fails the transfer cleanly when
temporary byte or wide-character buffers cannot be allocated.

Regression coverage: `tests/automated/check-textfield-dostuff-length.py`.
