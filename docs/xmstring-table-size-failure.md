---
nav_exclude: true
---
# XmString-table destination-size failure

`CvtStringToXmStringTable` now frees every generated `XmString` and its table
when the destination buffer is too small. The converter still reports the
required pointer size and returns failure, but no longer leaks the successful
intermediate allocation on that error path.

`tests/automated/check-xmstring-table-size-failure.py` protects this contract.
