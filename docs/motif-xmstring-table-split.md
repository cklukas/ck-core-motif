# XmString table split allocation failure

`XmStringToXmStringTable` now checks both result-table allocations and passes
the bounded counted-table size explicitly as an Xt `Cardinal` before writing
entries. On failure it releases the Motif process lock and returns zero,
leaving the caller with no partially initialized output table.

The regression check is `tests/automated/check-xmstring-table-split.py`.
