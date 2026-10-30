---
nav_exclude: true
---
# Container begin-extend input guards

The Container begin-extend action now returns safely when its event or
parameter-count pointer is missing.  This protects outline-button arming and
range-selection hit testing from malformed programmatic action calls while
leaving normal pointer-driven extension unchanged.

The source contract is checked by
`tests/automated/check-container-begin-extend-input-guards.py`.
