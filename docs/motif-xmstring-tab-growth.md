---
nav_exclude: true
---
# XmString tab-list growth allocation failure

While scanning strings, `XmStringTableProposeTablist` may create additional
tabs.  Each new tab is now checked before it is linked into the list.  On
failure, the string context and partial tab list are released, the process
lock is balanced, and `NULL` is returned.

The regression check is
`tests/automated/check-xmstring-tab-growth-allocation.py`.
