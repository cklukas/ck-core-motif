---
nav_exclude: true
---
# Container cell-size dispatch input validation

Cell-size dispatch now returns safely when no Container widget is supplied,
before querying spatial style or icon view state. Valid small- and large-cell
dispatch is unchanged.

The source contract is checked by
`tests/automated/check-container-cell-size-dispatch-inputs.py`.
