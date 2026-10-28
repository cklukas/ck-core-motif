# Bound drag target-table allocations

The binary-swap drag reader checks target-list and target-entry allocation
products against the `Cardinal` limit used by Xt allocation APIs.  A protocol
count that is representable in its 16-bit wire field but not in the local
allocator is rejected cleanly instead of wrapping the allocation size.  The
same validation is applied to each serialized atom list before its temporary
array is allocated; partial table growth is rolled back on failure.

Valid target tables are unchanged.  The focused source regression is
`tests/automated/check-drag-target-allocation-bounds.py`.
