---
nav_exclude: true
---
# Container hit-test input validation

Hit testing now returns no item when called without a Container widget, before
accessing the node list or layout state. Valid coordinate hit testing is
unchanged.

The source contract is checked by
`tests/automated/check-container-hit-test-inputs.py`.
