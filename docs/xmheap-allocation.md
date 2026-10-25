---
nav_exclude: true
---
# Legacy XmHeap allocation

The compatibility heap constructor now checks its private record allocation
before initializing the segment fields.  Segment allocation also checks its
size arithmetic and result before writing the heap link, and rejects segment
sizes too small to hold the bookkeeping pointer.  Allocation failure returns
`NULL` cleanly; existing callers can use their normal no-heap fallback, while
successful heap behavior is unchanged.

`tests/automated/check-xmheap-allocation.py` protects this contract.
