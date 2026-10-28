---
nav_exclude: true
---

# UIL value-entry dump pointer widths

Value-entry diagnostics now print the entry and name pointers with `%p` and
`void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-value-dump-pointers.py`
