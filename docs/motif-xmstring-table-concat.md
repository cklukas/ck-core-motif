---
nav_exclude: true
---
# XmString table concatenation input validation

`XmStringTableToXmString` now rejects a null table and empty table before
entering its concatenation loop.  The Motif process lock is released on this
path, and the function returns `NULL` instead of dereferencing a missing
table.

The regression check is
`tests/automated/check-xmstring-table-concat-input.py`.
