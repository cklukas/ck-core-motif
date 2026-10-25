---
nav_exclude: true
---
# Icon-file cache directory allocation

Derived icon cache entries now release their copied key and record when the
resolved directory-name buffer cannot be allocated.  The optional cache is
skipped without writing through null storage, while icon resolution continues.

The regression check is
`tests/automated/check-iconfile-dir-allocation.py`.
