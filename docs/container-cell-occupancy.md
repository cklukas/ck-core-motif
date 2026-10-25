# Container cell occupancy counts

Container removal decrements the occupancy count for the child cell.  A
partially rebuilt grid can temporarily disagree with a child's recorded cell,
so removal now decrements only a valid, positive counter.  Valid grids retain
the existing behavior while stale state cannot produce a negative occupancy
count that would make later placement treat a busy cell as free.

`tests/automated/check-container-cell-occupancy-underflow.py` protects this
invariant.
