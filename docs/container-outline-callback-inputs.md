---
nav_exclude: true
---
# Container outline callback input validation

The private outline-button callback now rejects missing client widgets and
callback payloads before looking up or dereferencing the child constraint. It
also safely handles a child without a constraint record.

The source contract is checked by
`tests/automated/check-container-outline-callback-inputs.py`.
