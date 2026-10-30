---
nav_exclude: true
---
# Container node deletion input validation

Node deletion now returns safely for a missing child widget or constraint
record before recursing through descendants. Valid recursive tree teardown is
unchanged.

The source contract is checked by
`tests/automated/check-container-delete-node-inputs.py`.
