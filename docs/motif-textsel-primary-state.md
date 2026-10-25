---
nav_exclude: true
---
# Text primary-selection state allocation failure

Primary-selection handling allocates a small state record before requesting
the selected target.  The allocation is now checked while the process lock is
held.  On failure, the lock is released, callback data is cleaned up, and the
paste request returns without dereferencing null state.

The regression check is
`tests/automated/check-textsel-primary-state-allocation.py`.
