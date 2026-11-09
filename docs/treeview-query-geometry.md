---
nav_exclude: true
---
# TreeView geometry negotiation

`XmTreeView` now tracks automatic sizing independently on each axis.  A zero
constructor or `XtSetValues` dimension requests content-derived sizing for that
axis.  A nonzero application-supplied width or height is retained in
`QueryGeometry`, so a geometry manager cannot replace it with the TreeView's
natural label or row metrics.
