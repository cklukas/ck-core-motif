---
nav_exclude: true
---

# Drag conversion failure cleanup

`XmeDragSource` records source and location data before asking the widget to
convert its export targets.  When conversion fails or returns an invalid
target list, the drag never starts; those context fields are now cleared on
both early exits.  This prevents a later Motif-drop conversion from observing
stale data from a failed drag attempt.

The regression check is
`tests/automated/check-drag-conversion-failure-cleanup.py`.
