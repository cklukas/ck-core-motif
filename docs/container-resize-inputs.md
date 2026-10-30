---
nav_exclude: true
---
# Container icon-width input validation

Icon-width resizing now returns safely for a missing Container widget and skips
malformed child nodes or missing constraint records in both measurement and
configuration passes. Valid layout behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-resize-inputs.py`.
