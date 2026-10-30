---
nav_exclude: true
---
# Container spatial-size adjustment input validation

Spatial-size adjustment now returns safely for missing output pointers, an
invalid item count, or a missing item array when items are requested. Valid
cell-placement calculations are unchanged.

The source contract is checked by
`tests/automated/check-container-spatial-adjustment-inputs.py`.
