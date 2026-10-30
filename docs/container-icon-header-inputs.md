---
nav_exclude: true
---
# Container icon-header input validation

Icon-header creation now returns safely for a missing Container widget and
skips optional scrolled-parent trait probing when the parent is unavailable.
Normal header creation and scrolled-container integration are unchanged.

The source contract is checked by
`tests/automated/check-container-icon-header-inputs.py`.
