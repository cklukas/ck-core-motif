---
nav_exclude: true
---
# Container button-up parameter guards

The Container button-up action now handles a missing action-parameter count as
an invalid invocation before dereferencing it.  Valid Xt action dispatch keeps
the existing timer, drag-cancellation, and follow-up action behavior.

The source contract is checked by
`tests/automated/check-container-button-up-parameter-guards.py`.
