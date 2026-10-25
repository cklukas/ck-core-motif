---
nav_exclude: true
---
# XmString table parse allocation failure

`XmStringTableParseStringArray` allocates the result table before parsing each
input string.  The allocation is now checked while the Motif process lock is
held.  If it fails, the lock is released and the function returns `NULL`
without writing through invalid storage.

The regression check is
`tests/automated/check-xmstring-table-parse-allocation.py`.
