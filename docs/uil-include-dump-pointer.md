---
nav_exclude: true
---

# UIL include-file dump pointer width

Include-file symbol diagnostics now print their entry address with `%p` and
`void *`, preserving full pointer values on 64-bit hosts.

Regression: `tests/automated/check-uil-include-dump-pointer.py`
