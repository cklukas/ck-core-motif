---
nav_exclude: true
---

# Invalid clipboard snapshot cleanup

When deferred clipboard setup receives a snapshot result with an invalid
count or format, the result buffer is now released before the clipboard copy
is ended.  This complements the atom-format validation and prevents malformed
snapshot conversions from leaking an allocated value.

The regression check is
`tests/automated/check-clipboard-snapshot-cleanup.py`.
