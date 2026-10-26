# RowColumn geometry allocation guards

`_XmRCGetKidGeo` now checks the managed-child count, tear-off addition, and
geometry-record multiplication before allocating its result array. Allocation
failure returns `NULL` before any record is dereferenced, allowing callers to
abort the geometry pass safely instead of crashing or wrapping the requested
size.

The focused source-contract check is
`tests/automated/check-rc-layout-geometry-allocation.py`.
