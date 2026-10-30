---
nav_exclude: true
---
# Container marked-state constraint guards

`SetMarkedCwids` now skips nodes whose Container constraint record is
unavailable while synchronizing selection state.  This protects selection
cleanup during child teardown; valid nodes retain their existing state
transitions.

The source contract is checked by
`tests/automated/check-container-set-marked-constraint-guards.py`.
