---
nav_exclude: true
---

# UIL control dump pointer widths

Control diagnostics now print the controlled-object pointer with `%p` and
`void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-control-dump-pointers.py`
