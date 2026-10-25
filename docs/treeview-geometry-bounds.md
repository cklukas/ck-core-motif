---
nav_exclude: true
---
# TreeView geometry bounds

TreeView preferred height now computes row-height multiplication in a wider
type and clamps it to the platform `Dimension` maximum. Large but valid
`XmNtreeViewRowHeight` and `XmNtreeViewVisibleRows` values no longer wrap into
an unexpectedly small geometry request; ordinary sizes are unchanged.

`tests/automated/check-treeview-geometry-bounds.py` protects this calculation.
