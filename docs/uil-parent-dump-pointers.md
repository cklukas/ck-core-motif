---
nav_exclude: true
---

# UIL parent-list dump pointer widths

Parent-list diagnostics now print the entry, parent, and next-link pointers
with `%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-parent-dump-pointers.py`
