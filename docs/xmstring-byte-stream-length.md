---
nav_exclude: true
---
# XmString byte-stream length validation

`XmStringByteStreamLength` now checks the compound-string ASN.1 header before
reading length bytes. Non-conforming input returns zero instead of interpreting
arbitrary memory as a length; valid byte streams retain their existing result.

The regression check is
`tests/automated/check-xmstring-byte-stream-length.py`.
