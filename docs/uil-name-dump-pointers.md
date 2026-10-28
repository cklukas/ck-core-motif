---
nav_exclude: true
---

# UIL name-entry dump pointer widths

Name-entry diagnostics now print the entry, next-name, and object pointers
with `%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-name-dump-pointers.py`
