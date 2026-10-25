---
nav_exclude: true
---
# RowColumn traversal child-list bounds

`TraversalChildren` now initializes its output parameters, checks the
`(num_children + 1) * sizeof(Widget)` size against both `size_t` and Xt's
`Cardinal` allocator width, and handles allocation failure without writing
through a null result. The bounded byte count is passed explicitly as an Xt
`Cardinal`. The tear-off control remains the first entry, followed
by the original composite children.

The focused source regression is
`tests/automated/check-rowcolumn-traversal-children.py`.
