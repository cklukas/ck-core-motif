# Hierarchy list allocation

Hierarchy node-table and child-list growth now allocates into temporary
pointers and publishes the replacement only after `XtRealloc` succeeds. Size
products and `Cardinal` growth are checked before allocation. A failed growth
leaves the existing list available and skips the operation instead of losing
the old pointer or dereferencing null storage.

The regression check is
`tests/automated/check-hierarchy-growth-allocation.py`.
