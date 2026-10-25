---
nav_exclude: true
---
# Mrm node-space validation

`Idb__INX_ConfirmNodeSpace` now requires a node record and validates its count,
heap offset, free-space range, and vector footprint before deciding whether a
split is needed.  Corrupt metadata is returned as `MrmBAD_RECORD` instead of
being treated as available space.

The source-level regression is
`tests/automated/check-mrm-confirm-node-space.py`.
