---
nav_exclude: true
---
# Column label-copy rollback

`XmColumn` now treats a failed copy of a replacement label string as a
transaction failure. The existing child label remains owned by the current
constraint record, and the candidate record reuses that value instead of
freeing it and passing a null or dangling string to the child widget.

The source contract is checked by
`tests/automated/check-column-label-allocation.py`, alongside the generated
label-name and child-creation failure paths.
