---
nav_exclude: true
---
# Container cell-grid reset

`PlaceItemReset` now validates cell metrics before dividing, rejects an
unrepresentable cell-grid product, verifies the occupancy-array byte size
against both host `size_t` and Xt's `Cardinal` allocation limit, and checks
the new allocation.  A failed reset clears the active grid dimensions and
returns without publishing a partially initialized cell layout, so later
placement cannot dereference a missing grid.

`tests/automated/check-container-cell-reset.py` protects these checks.
