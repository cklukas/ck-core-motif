---
nav_exclude: true
---
# Container size input validation

Container natural-size calculation now returns safely when its widget or
dimension outputs are absent, and skips malformed child nodes or missing
constraint records. Normal geometry negotiation is unchanged.

The source contract is checked by
`tests/automated/check-container-size-inputs.py`.
