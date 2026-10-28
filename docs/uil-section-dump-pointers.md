---
nav_exclude: true
---

# UIL section-dump pointer widths

Section diagnostics now print the section, previous/next links, and entry-list
pointer with `%p` and `void *`, preserving full addresses on 64-bit hosts.

Regression: `tests/automated/check-uil-section-dump-pointers.py`
