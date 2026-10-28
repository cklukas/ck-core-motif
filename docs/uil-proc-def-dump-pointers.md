---
nav_exclude: true
---

# UIL procedure-definition dump pointer widths

Procedure-definition diagnostics now print the entry and name pointers with
`%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-proc-def-dump-pointers.py`
