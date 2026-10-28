---
nav_exclude: true
---
# List drag selection bounds

List drag-and-drop snapshots are built from the widget's selected-position
array.  That array can contain stale entries during selection updates, so the
snapshot now validates every one-based position against the current item
count, compacts the valid entries, and falls back to the item that initiated
the drag when no valid selected position remains.  Valid multi-selection and
the public drag protocol are unchanged.

The source contract is checked by
`tests/automated/check-list-drag-selection-bounds.py`.
