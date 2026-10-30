---
nav_exclude: true
---
# Container outline-hiding input validation

The outline-hiding helper now returns safely for a child without a Container
constraint and skips malformed child-list entries before recursive hiding.
Valid outline visibility behavior is unchanged.

The source contract is checked by
tests/automated/check-container-hide-inputs.py.
