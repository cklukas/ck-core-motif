---
nav_exclude: true
---

# UIL case-sensitivity tag width

The module case-sensitivity flag is a boolean value stored in the symbol
header's 8-bit tag field. The conversion is now explicit, documenting the
intent and keeping strict warning builds free of an incidental narrowing
diagnostic without changing the stored `0`/`1` values.

Regression: `tests/automated/check-uil-case-sensitivity-tag-width.py`
