---
nav_exclude: true
---
# Dynamic Container first-column input validation

Dynamic first-column sizing now returns safely for a missing Container widget
and skips malformed child nodes or missing constraint records. Valid icon
measurement and column sizing remain unchanged.

The source contract is checked by
`tests/automated/check-container-first-column-inputs.py`.
