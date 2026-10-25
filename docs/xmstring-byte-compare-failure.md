---
nav_exclude: true
---
# XmStringByteCompare conversion failure

`XmStringByteCompare` now checks both ASN.1 conversion results before calling
`memcmp` or releasing the buffers. A failed conversion returns `False` instead
of allowing null or undefined pointers to reach the comparison path. Normal
comparison and null-input behavior are unchanged.

The regression check is
`tests/automated/check-xmstring-byte-compare-failure.py`.
