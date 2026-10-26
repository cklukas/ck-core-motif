---
nav_exclude: true
---
# Container CELLS cell-size guard

The spatial CELLS resize path divides child dimensions by the computed cell
width and height.  A zero metric can occur before cell metrics are established
or after an invalid resource update.  The path now initializes both values,
rejects each requested zero dimension before it is used, and only then
performs the divisions.  GRID behavior and public resource compatibility are
unchanged.

The focused source contract is checked by
`tests/automated/check-container-cell-size-guard.py`.
