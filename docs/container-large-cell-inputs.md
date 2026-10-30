---
nav_exclude: true
---
# Container large-cell input validation

Large-cell sizing now returns safely for a missing Container widget and falls
back to screen-based defaults when the node list contains no usable child
widget. Valid dynamic sizing remains unchanged.

The source contract is checked by
`tests/automated/check-container-large-cell-inputs.py`.
