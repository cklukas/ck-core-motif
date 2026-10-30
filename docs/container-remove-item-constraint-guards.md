---
nav_exclude: true
---
# Container removal constraint guards

`RemoveItem` now returns `False` when the child has no Container constraint
record before checking placement or updating occupancy.  Valid grid, cell, and
none-style removal remains unchanged.

The source contract is checked by
`tests/automated/check-container-remove-item-constraint-guards.py`.
