---
nav_exclude: true
---

# Clipboard format-length property request bound

Clipboard format metadata stores its length in the first 32-bit property
value. `ClipboardGetLenFromFormat` now requests one property unit instead of a
large fixed maximum, reducing memory and protocol work for malformed or
unexpectedly large properties while preserving the value used by valid
clipboard clients.

The focused regression check is
`tests/automated/check-clipboard-format-length-request.py`.
