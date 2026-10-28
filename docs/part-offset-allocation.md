---
nav_exclude: true
---
# Widget part-offset allocation

`_XiResolveAllPartOffsets` now validates its required inputs and superclass
before reading inherited widget sizes or allocating an offset table. It also
checks the offset-table size before allocation. If either table cannot be
allocated, partial state is released and the Motif process lock is released
before returning; resource offsets are not updated from an incomplete table.
Alignment rounding and widget/constraint-size addition are checked before
class records are changed, so oversized layouts fail without partial size
updates.

`tests/automated/check-part-offset-allocation.py` protects this contract.
