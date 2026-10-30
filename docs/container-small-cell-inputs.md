---
nav_exclude: true
---
# Container small-cell input validation

Small-cell sizing now returns safely for a missing Container widget and falls
back to screen-based defaults when the node list contains no usable child
widget. Valid dynamic sizing remains unchanged.

The source contract is checked by
`tests/automated/check-container-small-cell-inputs.py`.
