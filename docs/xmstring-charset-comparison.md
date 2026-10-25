---
nav_exclude: true
---

# XmString charset comparison

`_XmStringIsCurrentCharset` treats a `NULL` charset tag as a non-match. This
keeps optional or malformed application metadata from reaching `strcmp` and
crashing the process, while preserving the existing comparison for valid
charset strings.

The focused regression check is
`tests/automated/check-xmstring-charset-null.py`.
