---
nav_exclude: true
---
# Container default-action callback input validation

The Container default-action callback now returns safely when its child,
parent, or constraint record is unavailable during teardown or unusual
dispatch. Valid default-action callbacks are unchanged.

The source contract is checked by
tests/automated/check-container-action-callback-inputs.py.
