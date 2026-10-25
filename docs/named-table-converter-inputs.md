---
nav_exclude: true
---

# Named-table converter input validation

Button-type, keysym-table, and charset-table converters now validate source and
destination values (and required displays) before parsing. Their cleanup
callbacks also tolerate missing destination addresses. Valid table conversion
and cleanup remain unchanged.

The focused regression check is
`tests/automated/check-named-table-converter-inputs.py`.
