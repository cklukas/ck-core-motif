---
nav_exclude: true
---
# TreeView row-height bounds

`XmNtreeViewRowHeight` is a public `Dimension` resource.  A zero value left
the widget with zero-height rows after initialization or `SetValues`, making
the view effectively unusable even though `visibleRows` was already guarded.
Both lifecycle paths now clamp row height to one pixel before geometry and
input calculations.

`tests/automated/check-tree-view-row-height.py` keeps the invariant in both
paths.
