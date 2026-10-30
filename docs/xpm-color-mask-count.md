---
nav_exclude: true
---
# XPM masked color-table validation

`ScanOtherColors` now validates color and pixel arrays before applying the
transparent-mask offset.  An empty masked table can no longer underflow its
unsigned color count or advance null pointers; malformed non-empty inputs are
also rejected before allocation and lookup.

The regression check is `tests/automated/check-xpm-color-mask-count.py`.
