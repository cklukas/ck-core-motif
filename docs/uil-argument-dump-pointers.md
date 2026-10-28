---
nav_exclude: true
---

# UIL argument dump pointer widths

Argument diagnostics now print argument-name and argument-value pointers with
`%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-argument-dump-pointers.py`
