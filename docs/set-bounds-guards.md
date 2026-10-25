---
nav_exclude: true
---
# Icon-box bounds recalculation guards

`SetNewBounds` now handles partially initialized icon boxes and invalid icon
increments by resetting the tracked last row/column to an empty state.  It
avoids dereferencing missing widgets and divide-by-zero during layout updates.

`tests/automated/check-set-bounds-guards.py` protects this path.
