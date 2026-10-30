---
nav_exclude: true
---
# Container select-all input validation

Select-all now returns safely when no Container widget is supplied, before
reading selection state or traversing nodes. Valid selection behavior is
unchanged.

The source contract is checked by
`tests/automated/check-container-select-all-inputs.py`.
