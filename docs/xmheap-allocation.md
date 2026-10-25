---
nav_exclude: true
---
# Legacy XmHeap allocation

The compatibility heap constructor now checks its private record allocation
before initializing the segment fields.  Allocation failure returns `NULL`
cleanly; existing callers can use their normal no-heap fallback, while
successful heap behavior is unchanged.

`tests/automated/check-xmheap-allocation.py` protects this contract.
