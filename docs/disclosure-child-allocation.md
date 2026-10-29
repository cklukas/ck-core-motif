---
nav_exclude: true
---
# Disclosure child allocation

Disclosure creates indicator, header, and content children during
initialization.  Geometry, state transitions, callbacks, and resource updates
now tolerate an individual child failing to allocate, so later redraws and
resource changes remain safe.

The contract is protected by
`tests/automated/check-disclosure-child-failure-guards.py`.
