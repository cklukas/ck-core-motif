# Container cell index bounds

Container coordinate conversion can briefly see an invalid cell while a
closest-placement request or geometry reset is in progress.  Closest
placement now rejects negative and still-out-of-range cells before indexing
the occupancy array.  Cell-to-coordinate conversion and `SnapCwid` also
return the container margin origin for invalid cells, preventing negative
coordinates and stale-grid dereferences while preserving valid placement.

`tests/automated/check-container-cell-index-bounds.py` protects these
contracts.
