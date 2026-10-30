---
nav_exclude: true
---
# Container outline layout input validation

Outline layout now returns safely for a missing Container widget, skips
malformed child or constraint records, and omits connector lines whose parent
constraint is unavailable. Valid outline geometry and connector rendering are
unchanged.

The source contract is checked by
`tests/automated/check-container-outline-layout-inputs.py`.
