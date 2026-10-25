---
nav_exclude: true
---
# List internal element allocation failure

List item insertion now handles failure while constructing an internal element
record.  Any records already allocated for the insertion are released, the
internal pointer array is restored to its original order, and cached extents
are restored before the operation returns.  This keeps the existing list
usable instead of dereferencing null storage or leaving a partial insertion.

The regression check is
`tests/automated/check-list-element-allocation.py`.
