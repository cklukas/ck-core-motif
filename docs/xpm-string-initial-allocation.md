---
nav_exclude: true
---

# XPM string initial allocation

File-backed `xpmGetString` now checks its initial one-byte allocation before
entering the record-growth loop, returning `XpmNoMemory` directly instead of
depending on `XpmRealloc(NULL, ...)` behavior.

Regression: `tests/automated/check-xpm-string-initial-allocation.py`
