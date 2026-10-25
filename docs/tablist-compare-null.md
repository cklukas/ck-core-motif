---
nav_exclude: true
---
# TabList comparison with absent lists

`XmTabbedStackListCompare` now treats two `NULL` lists as equal and a single
`NULL` list as a size mismatch.  It returns before reading list fields, so
callers can compare optional tab-list values safely.

The focused regression is `tests/automated/check-tablist-compare-null.py`.
