---
nav_exclude: true
---
# Container hit-test input validation

Container hit-testing now skips malformed child-list entries and children
without a constraint record before reading geometry or outline state. Valid
children retain the existing hit-test behavior.

The source contract is checked by
`tests/automated/check-container-hit-test-inputs.py`.
