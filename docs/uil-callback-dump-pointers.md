---
nav_exclude: true
---

# UIL callback dump pointer widths

Callback diagnostics now print reason-name, procedure-reference, and
procedure-reference-list pointers with `%p` and `void *`, preserving complete
addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-callback-dump-pointers.py`
