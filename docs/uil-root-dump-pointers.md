---
nav_exclude: true
---

# UIL root-entry dump pointer widths

Root-entry diagnostics now print the root, module, and section pointers with
`%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-root-dump-pointers.py`
