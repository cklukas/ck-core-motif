---
nav_exclude: true
---
# Container cell-position bounds

Spatial cell coordinates are ultimately stored in X11 `Position`/`XPoint`
fields.  `GetCoordFromCell` now uses saturating products for the cell index,
cell dimension, and margin instead of allowing large grids to wrap into a
negative or unrelated location.  Invalid or incomplete grids retain the
existing safe fallback coordinate.

The focused source contract is covered by
`tests/automated/check-container-cell-position.py`.
