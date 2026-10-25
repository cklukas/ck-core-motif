---
nav_exclude: true
---

# Resource-name comparison input validation

`XmeNamesAreEqual` now returns `False` when either input string is missing,
avoiding an invalid dereference. Existing Motif-prefix and case-insensitive
comparisons remain unchanged for valid strings.

The focused regression check is
`tests/automated/check-name-compare-inputs.py`.
