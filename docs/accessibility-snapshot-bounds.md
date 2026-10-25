---
nav_exclude: true
---
# Accessibility text snapshot allocation

The D-Bus accessibility transport builds a bounded UTF-8 snapshot before
comparing text changes. The snapshot buffer allocation is now checked before
it is used, so an allocation failure abandons the snapshot cleanly instead of
writing through a null pointer. Normal text snapshots and their lifecycle are
unchanged.

The source-level regression is
`tests/automated/check-accessible-snapshot-allocation.py`.
