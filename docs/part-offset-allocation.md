---
nav_exclude: true
---
# Widget part-offset allocation

`_XiResolveAllPartOffsets` now validates its required inputs and offset-table
size before allocation. If either table cannot be allocated, partial state is
released and the Motif process lock is released before returning; resource
offsets are not updated from an incomplete table.

`tests/automated/check-part-offset-allocation.py` protects this contract.
