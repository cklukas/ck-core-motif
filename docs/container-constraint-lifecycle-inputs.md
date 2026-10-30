---
nav_exclude: true
---
# Container constraint lifecycle input validation

Container child initialization now handles a missing parent constraint without
dereferencing it, and constraint destruction returns safely when its child or
parent state is unavailable. Valid hierarchy initialization and teardown are
unchanged.

The source contract is checked by
`tests/automated/check-container-constraint-lifecycle-inputs.py`.
