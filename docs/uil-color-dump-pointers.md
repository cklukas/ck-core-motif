---
nav_exclude: true
---

# UIL color-item dump pointer widths

Color-item diagnostics now print the entry, color, and next-item pointers with
`%p` and `void *`, preserving complete addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-color-dump-pointers.py`
