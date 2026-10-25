---
nav_exclude: true
---

# Font-list converter input validation

The string-to-font-list converter now returns `False` when its source or
destination `XrmValue` is missing, avoiding invalid dereferences. Valid font
list parsing, fallback loading, and conversion warnings remain unchanged.

The focused regression check is
`tests/automated/check-fontlist-converter-inputs.py`.
