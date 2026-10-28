---
nav_exclude: true
---

# UIL object-variant dump pointer width

Default-object-variant diagnostics now print the entry and next-link pointers
with `%p` and `void *`, preserving complete addresses on 64-bit systems.

Regression: `tests/automated/check-uil-object-variant-dump-pointer.py`
