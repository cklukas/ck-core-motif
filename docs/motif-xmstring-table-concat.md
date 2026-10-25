# XmString table concatenation input validation

`XmStringTableToXmString` now rejects a null table or an empty table while the
Motif process lock is held.  The lock is released before returning `NULL`, so
invalid input cannot enter the concatenation loop.

The regression check is
`tests/automated/check-xmstring-table-concat-input.py`.
