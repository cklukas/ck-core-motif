---
nav_exclude: true
---
# Gadget cache allocation handling

`_XmCachePart` now checks cache-record size arithmetic and both cache-record
allocations before linking or copying data.  Failed allocations leave the
class cache unchanged and return `NULL`, preventing null cache-record
initialization while preserving normal cache sharing on success.

The regression check is `tests/automated/check-cache-part-allocation.py`.
