---
nav_exclude: true
---

# UIL widget-dump pointer widths

The symbol-table widget dumper is also used when diagnosing compiler input on
64-bit systems. Its pointer fields now use `%p` and `void *`, so debug output
does not truncate addresses to 32 bits.

Regression: `tests/automated/check-uil-widget-dump-pointers.py`
