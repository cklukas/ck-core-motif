---
nav_exclude: true
---

# Zero-sized compound-string conversions

COMPOUND_STRING export now rejects a zero-sized
`XmCvtXmStringToByteStream()` result. A valid XmString always includes an
ASN.1 header, so zero indicates conversion or allocation failure and must not
be emitted as an empty property.

The focused regression check is
`tests/automated/check-txtpropcv-asn1-zero.py`.
