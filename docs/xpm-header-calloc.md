---
nav_exclude: true
---

# XPM header-table allocation

The temporary pointer table used while serializing an `XpmImage` is now
allocated with `header_nlines` elements.  The previous code passed the table's
byte count as the element count, causing an unnecessary extra multiplication
inside `calloc`; the resulting table contents and public behavior are
unchanged.

Regression: `tests/automated/check-xpm-header-calloc.py`
