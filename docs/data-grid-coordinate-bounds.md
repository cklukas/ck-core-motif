---
nav_exclude: true
---

# DataGrid coordinate bounds

DataGrid keeps its column and row layout calculations in signed `int`
arithmetic, then clamps cell-label coordinates before passing them to Motif's
signed `Position` drawing arguments.  Large grids and row heights therefore
cannot wrap text to an unrelated screen coordinate.

`tests/automated/check-data-grid-coordinate-bounds.py` protects this drawing
contract.
