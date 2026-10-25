---
nav_exclude: true
---
# Container cell placement guards

Container placement and hit testing can run while a geometry reset is
incomplete, or after a coordinate falls outside the current cell grid.  The
occupancy array is now checked before placement and removal, and hit testing
rejects `NO_CELL` and out-of-range indices before indexing it.  This keeps a
failed allocation or transient layout state from becoming a null or
out-of-bounds access while preserving the existing placement result for valid
grids.

`tests/automated/check-container-cell-placement-guards.py` protects these
contracts.
