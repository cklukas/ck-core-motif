---
nav_exclude: true
---

# XmString ASN.1 truncation boundaries

`_XmStringTruncateASN1` now checks the component pointer against the encoded
string end before reading a component header. This prevents an out-of-bounds
read when the final component ends exactly at the truncation boundary while
preserving the existing short-header compaction behavior. It also rejects
component lengths that would extend beyond the encoded buffer before calling
the component reader.

The focused regression check is
`tests/automated/check-xmstring-truncate-boundary.py`.
