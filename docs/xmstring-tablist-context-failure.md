---
nav_exclude: true
---
# XmString tab-list traversal failure

`XmStringTableProposeTablist` now checks the string context after each input
has been scanned. If traversal reports an internal representation error, the
partial tab list is released and the function returns `NULL`; callers no
longer receive a silently incomplete proposal.

The regression check is
`tests/automated/check-xmstring-tablist-context-failure.py`.
