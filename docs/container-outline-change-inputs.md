---
nav_exclude: true
---
# Container outline-button refresh input validation

Outline-button refresh now returns safely when no Container widget is supplied,
before traversing child constraints or updating pixmaps. Valid refresh behavior
is unchanged.

The source contract is checked by
`tests/automated/check-container-outline-change-inputs.py`.
