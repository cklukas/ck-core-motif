---
nav_exclude: true
---

# TabStack label getter guard

The TabStack constraint-resource getter now validates its widget and output
storage before copying a child tab label. Invalid synthetic-resource callback
inputs return a deterministic null value instead of dereferencing a missing
constraint record or output slot. Normal valid child-resource reads still use
the existing copied `XmString` contract.

The focused source-contract check is
`python3 tests/automated/check-tabstack-label-guard.py`.
