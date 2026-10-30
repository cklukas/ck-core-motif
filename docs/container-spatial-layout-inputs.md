---
nav_exclude: true
---
# Container spatial layout input validation

Spatial layout now returns safely for a missing Container widget and skips
malformed nodes without dereferencing a missing child widget. Valid placement,
growth, and recursion behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-spatial-layout-inputs.py`.
