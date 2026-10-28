---
nav_exclude: true
---

# UIL table-count bounds

UIL value entries store table counts in an 8-bit field. The semantic table
builder now rejects a count above `UCHAR_MAX` before narrowing, preventing a
large table from silently wrapping its recorded count.

Regression: `tests/automated/check-uil-table-count-bounds.py`
