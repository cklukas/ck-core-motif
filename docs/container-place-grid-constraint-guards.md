---
nav_exclude: true
---
# Container grid/cell placement constraint guards

`PlaceItemGridCells` now returns when the candidate child has no Container
constraint record.  Grid and cell placement require the record's logical
coordinates for closest-cell selection and snapping; valid placement is
unchanged.

The source contract is checked by
`tests/automated/check-container-place-grid-constraint-guards.py`.
