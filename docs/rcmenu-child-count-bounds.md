---
nav_exclude: true
---
# RowColumn menu child-count traversal

RowColumn menu activation, recursive menu processing, menu search, and widget
membership checks now use `Cardinal` indices when traversing composite
children. This matches Xt's child-count field and avoids signed-index
wraparound for large valid menu hierarchies.

The focused source regression is
`tests/automated/check-rcmenu-child-count-bounds.py`.
