---
nav_exclude: true
---
# Container sizing constraint guards

Closest-layout sizing now skips a node when its Container constraint record is
unavailable instead of reading `user_x` or `user_y` through a null pointer.
Other nodes continue through the normal sizing pass, and valid closest-layout
coordinates are unchanged.

The source contract is checked by
`tests/automated/check-container-size-constraint-guards.py`.
