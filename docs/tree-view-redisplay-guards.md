# TreeView redisplay guards

`XmTreeView` now skips redisplay when its drawing GC is unavailable, treats null
item labels as empty strings, and clamps selected-row and focus rectangles for
narrow or short widgets. This prevents null-string faults and unsigned X11
geometry underflow while preserving ordinary tree rendering.

The focused source contract is checked by
`tests/automated/check-tree-view-redisplay-guards.py`.
