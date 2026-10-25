---
nav_exclude: true
---
# XmString table splitting allocation and indexing

`XmStringToXmStringTable` now checks both output-table allocations and
balances the process lock when allocation fails.  The no-break-component path
also writes the copied string through `(*table)[0]`, fixing the old precedence
error that wrote through an uninitialized pointer.

The regression check is
`tests/automated/check-xmstring-table-split.py`.
