---
nav_exclude: true
---

# String-to-`XmString` converter input validation

The compound-string converter now returns `False` when its source or
destination `XrmValue` is missing, avoiding an invalid dereference. Valid
string generation and converter cleanup behavior remain unchanged.

The focused regression check is
`tests/automated/check-xmstring-converter-inputs.py`.
