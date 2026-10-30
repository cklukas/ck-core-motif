---
nav_exclude: true
---
# Container begin-select input guards

The Container begin-selection action now returns safely when its event or
parameter-count pointer is missing.  This protects outline-button hit testing
and callback dispatch from malformed programmatic action calls while leaving
normal pointer-driven selection unchanged.

The source contract is checked by
`tests/automated/check-container-begin-select-input-guards.py`.
