---
nav_exclude: true
---

# UIL value-payload dump pointer widths

Value-payload diagnostics now print table links, user-defined charset values,
icon tables, and color entries with `%p` and `void *`, preserving complete
addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-value-payload-dump-pointers.py`
