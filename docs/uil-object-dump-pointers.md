---
nav_exclude: true
---

# UIL object-header dump pointers

The common symbol-object dumper reports names, references, and list links.
Those diagnostics now use `%p` and `void *`, avoiding 32-bit truncation when
debugging symbol tables on 64-bit hosts.

Regression: `tests/automated/check-uil-object-dump-pointers.py`
