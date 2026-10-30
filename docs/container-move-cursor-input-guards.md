---
nav_exclude: true
---
# Container cursor-movement input guards

The Container cursor-movement action now rejects missing events, parameter
counts, empty parameter lists, and null parameter storage before using the
direction token or forwarding keyboard selection.  Normal keyboard traversal
is unchanged.

The source contract is checked by
`tests/automated/check-container-move-cursor-input-guards.py`.
