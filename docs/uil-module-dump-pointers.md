---
nav_exclude: true
---

# UIL module-entry dump pointer widths

Module-entry diagnostics now print the entry, name, and version pointers with
`%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-module-dump-pointers.py`
