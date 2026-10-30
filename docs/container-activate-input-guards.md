---
nav_exclude: true
---
# Container activation input guards

The Container activate action now returns safely when its event or
parameter-count pointer is missing before forwarding outline-button or item
activation.  Normal keyboard and pointer activation remains unchanged.

The source contract is checked by
`tests/automated/check-container-activate-input-guards.py`.
