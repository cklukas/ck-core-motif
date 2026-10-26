# Bound drag receiver-info growth

Drag contexts grow their receiver-info array as windows are visited.  Growth
now checks the doubling arithmetic and the `Cardinal` allocation limit, and
handles `XtRealloc` failure without replacing the existing array.  Callers
abort the current receiver discovery cleanly when no new entry can be
allocated.

This preserves normal receiver discovery while preventing overflow or null
dereferences under allocation pressure.  The focused source regression is
`tests/automated/check-drag-receiver-info-allocation.py`.
