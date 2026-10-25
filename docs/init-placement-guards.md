---
nav_exclude: true
---
# Icon placement initialization guards

`InitIconPlacement` now rejects missing workspace data and non-positive icon
dimensions before calculating screen grid sizes.  It disables automatic icon
placement through the existing failure path instead of dividing by zero.

`tests/automated/check-init-placement-guards.py` protects this startup path.
