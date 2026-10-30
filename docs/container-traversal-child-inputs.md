---
nav_exclude: true
---
# Container traversal child validation

Next and previous traversal now return safely when the focus child has no
Container constraint or node record. Detached children cannot crash keyboard
focus traversal during teardown or unusual focus transitions.

The source contract is checked by
`tests/automated/check-container-traversal-child-inputs.py`.
