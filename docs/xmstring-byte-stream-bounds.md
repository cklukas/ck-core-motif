---
nav_exclude: true
---
# XmString byte-stream bounds validation

`XmCvtByteStreamToXmString` now validates every component header and declared
payload against the outer compound-string length. Truncated or inconsistent
streams return `NULL` instead of reading beyond the supplied byte stream;
well-formed streams retain their existing conversion behavior.

The regression check is
`tests/automated/check-xmstring-byte-stream-bounds.py`.
