---
nav_exclude: true
---
# XPM RGB lookup allocation

RGB-name lookup duplicates the caller's input before normalization. If that
allocation fails, lookup now returns failure instead of dereferencing a null
temporary buffer.

The source contract is covered by
`tests/automated/check-xpm-rgb-allocation.py`.
