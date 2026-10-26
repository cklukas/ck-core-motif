---
nav_exclude: true
---
# TextField locale-converted insertion lengths

TextField insertion now checks the size_t length returned by locale
conversion before converting it to the legacy int character count. The
wide-character allocation path also checks the length before adding its
terminator, preventing truncation and size arithmetic overflow.

Regression coverage: tests/automated/check-textfield-insert-locale-length.py.
