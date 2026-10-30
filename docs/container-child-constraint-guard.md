---
nav_exclude: true
---
# Container child constraint guard

`XmContainerGetItemChildren` now rejects a child whose constraint storage is
missing before reading its Container constraint record. The existing parent
check remains in place, and valid child enumeration is unchanged.

The source contract is checked by
`tests/automated/check-container-child-constraint-guard.py`.
