---
nav_exclude: true
---

# UIL external-definition dump pointer widths

External-definition diagnostics now print the entry, next-object, and name
pointers with `%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-external-dump-pointers.py`
