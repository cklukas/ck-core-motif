---
nav_exclude: true
---
# Container max-depth input validation

The optional max-depth traversal now returns safely for missing container or
constraint state and skips malformed child nodes. Valid depth accumulation is
unchanged when `FIX_1425` is enabled.

The source contract is checked by
`tests/automated/check-container-max-depth-inputs.py`.
