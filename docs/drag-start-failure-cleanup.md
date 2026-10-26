---
nav_exclude: true
---

# Drag-start failure cleanup

`XmeDragSource` stores the source widget and location payload in the Motif
drop conversion context before calling `XmDragStart`.  If the drag cannot be
created, the context now clears its payload immediately.  This prevents a
later conversion callback from observing stale source data for a drag that
never existed.

The regression check is
`tests/automated/check-drag-start-failure-cleanup.py`.
