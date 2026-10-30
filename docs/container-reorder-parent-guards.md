---
nav_exclude: true
---
# Container reorder parent guards

`XmContainerReorder` now verifies that the first child belongs to the target
Container and that every listed child has constraint storage before reading
its reorder metadata. Later entries from another parent are ignored, while a
foreign first entry aborts the operation without changing order. Valid
reorders are unchanged.

The source contract is checked by
`tests/automated/check-container-reorder-parent-guards.py`.
