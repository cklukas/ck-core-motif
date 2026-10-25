---
nav_exclude: true
---
# XmString tag-cache input validation

`_XmStringIndexCacheTag` now rejects null tags and negative lengths before
locking, comparing, or allocating cache entries. It returns the existing
sentinel index `0` for invalid input, preventing null string operations and
oversized length conversions.

The regression check is
`tests/automated/check-xmstring-tag-cache-input.py`.
