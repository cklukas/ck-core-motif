---
nav_exclude: true
---
# Container scroll timer input validation

The Container auto-scroll timer now returns safely when its closure is gone or
the reparented scroll-frame ancestor is unavailable during teardown. Normal
auto-scroll processing is unchanged when the hierarchy is intact.

The source contract is checked by
`tests/automated/check-container-scroll-inputs.py`.
