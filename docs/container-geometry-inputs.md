---
nav_exclude: true
---
# Container geometry input validation

Container geometry negotiation now rejects a missing child, request, parent, or
constraint record before reading widget state. Valid geometry requests keep
their existing negotiation behavior.

The source contract is checked by
`tests/automated/check-container-geometry-inputs.py`.
