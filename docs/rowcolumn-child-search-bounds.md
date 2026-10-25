---
nav_exclude: true
---
# RowColumn managed-child search bounds

`FindFirstManagedChild` now keeps both its scan index and copied composite
child count in `Cardinal` variables. This avoids truncating Xt's child count or
wrapping a signed index while finding the first traversable managed child.

The focused source regression is
`tests/automated/check-rowcolumn-child-search-bounds.py`.
