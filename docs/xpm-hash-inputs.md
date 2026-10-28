---
nav_exclude: true
---

# XPM hash interning inputs

Hash interning now rejects null tags and uninitialized tables with
`XpmFileInvalid` before hashing.  Valid initialized tables and non-null tags
retain the existing lookup and growth behavior.

Regression: `tests/automated/check-xpm-hash-inputs.py`
