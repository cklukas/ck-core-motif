---
nav_exclude: true
---

# MWM argument-vector allocation

MWM keeps private copies of the process argument vector during initialization,
including the WSM secondary-display copy. Both allocations now check the
argument count and pointer-array product against Xt's `Cardinal` width before
allocating. If the count is not representable, the existing fallback to the
original `argv` remains in effect.

The regression is covered by `tests/automated/check-mwm-argv-allocation.py`.
