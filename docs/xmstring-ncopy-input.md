---
nav_exclude: true
---
# XmStringNCopy input validation

`XmStringNCopy` now rejects a null source string or negative byte limit before
attempting ASN.1 conversion. This avoids passing invalid input to truncation
and conversion helpers while leaving valid bounded-copy behavior unchanged.

The regression check is
`tests/automated/check-xmstring-ncopy-input.py`.
