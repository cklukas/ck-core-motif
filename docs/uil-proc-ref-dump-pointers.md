---
nav_exclude: true
---

# UIL procedure-reference dump pointer widths

Procedure-reference diagnostics now print the entry, procedure-definition,
and argument-value pointers with `%p` and `void *`, preserving complete
addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-proc-ref-dump-pointers.py`
