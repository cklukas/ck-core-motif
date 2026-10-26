---
nav_exclude: true
---
# XPM RGB lookup allocation

RGB-name lookup duplicates the caller's input before normalization. If that
allocation fails, lookup now returns failure instead of dereferencing a null
temporary buffer.

Null input and output pointers are rejected before any string or result
access.

The RGB table binary search uses an overflow-safe midpoint calculation when
the table grows large.

The source contract is covered by
`tests/automated/check-xpm-rgb-allocation.py`.
