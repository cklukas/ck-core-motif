# XmString tab-list construction failure

`XmStringTableProposeTablist` now checks the initial tab and tab-list objects
before using them.  If either creation fails, the temporary tab is released,
the process lock is balanced, and the function returns `NULL` safely.

The regression check is
`tests/automated/check-xmstring-tablist-create.py`.
