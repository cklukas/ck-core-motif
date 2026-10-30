---
nav_exclude: true
---
# Container real-header input validation

Real icon-header lookup now returns safely for a missing Container widget, a
non-composite stored header, or a composite header with no child. Valid header
lookup remains unchanged.

The source contract is checked by
`tests/automated/check-container-real-header-inputs.py`.
