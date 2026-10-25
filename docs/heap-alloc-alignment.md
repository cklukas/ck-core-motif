---
nav_exclude: true
---

# Legacy heap allocation alignment

`_XmHeapAlloc` now validates and rounds requested sizes before deciding whether
the current segment has enough capacity. This prevents alignment arithmetic
from wrapping and keeps `bytes_remaining` from underflowing. Successful heap
allocation and alignment remain unchanged.

The focused regression check is
`tests/automated/check-heap-alloc-alignment.py`.
