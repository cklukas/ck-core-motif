---
nav_exclude: true
---

# UIL forward-reference dump pointer widths

Forward-reference diagnostics now print the entry, links, update location,
parent, and name pointers with `%p` and `void *`, preserving complete
addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-forward-ref-dump-pointers.py`
