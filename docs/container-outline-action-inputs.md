---
nav_exclude: true
---
# Container outline action state validation

The private outline action now returns safely when its child has no parent
Container or no constraint record, including teardown-time callback delivery.
Normal outline state transitions remain unchanged.

The source contract is checked by
`tests/automated/check-container-outline-action-inputs.py`.
