---
nav_exclude: true
---
# Container none-style placement constraint guards

`PlaceItemNone` now returns when the candidate child has no Container
constraint record.  Placement state (`cell_idx`) is private constraint data;
valid none-style placement remains unchanged.

The source contract is checked by
`tests/automated/check-container-place-none-constraint-guards.py`.
