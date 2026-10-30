---
nav_exclude: true
---
# Container keyboard-selection input validation

The Container keyboard-selection action now rejects missing widgets or events
before focus lookup, and safely handles a focused child without a constraint
record. Valid keyboard selection and primary-transfer behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-kbselect-inputs.py`.
