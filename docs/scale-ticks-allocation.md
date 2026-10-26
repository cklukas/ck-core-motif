---
nav_exclude: true
---
# Scale tick-array allocation failure

`XmScaleSetTicks` now checks the local array returned by `ALLOCATE_LOCAL` and
unlocks the application context before returning if allocation fails. This
prevents a low-memory condition from becoming a NULL write while preserving
normal tick generation.

The focused regression is
`tests/automated/check-scale-ticks-allocation.py`.
