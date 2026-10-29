---
nav_exclude: true
---
# Container fit edge arithmetic

`TestFitItem` rejects a candidate outside the right-to-left content edge before
it consults the occupancy grid.  Width and margin are unsigned Motif geometry
fields; subtracting them first and then casting to `Position` could wrap when a
transient allocation was smaller than its margin, causing an invalid candidate
to pass the edge test.

The right-to-left comparison now widens both operands before subtraction.  The
valid placement behavior is unchanged for ordinary allocations, while tiny
allocations are rejected deterministically.  The source contract is protected
by `tests/automated/check-container-cell-placement-guards.py`.
