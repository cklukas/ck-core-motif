# Container zero-sized cell guards

Container dynamic sizing can temporarily report a zero cell width or height
before a child has supplied measurements. Grid/cell layout now returns without
performing coordinate division until both dimensions are positive. Snapping
keeps the already computed point, while layout defers placement until a later
pass can measure the cells.

The focused regression is
`tests/automated/check-container-zero-cell-guards.py`. It checks both
`SnapCwid` and `LayoutSpatial` for guards before their cell-width and
cell-height divisions.
