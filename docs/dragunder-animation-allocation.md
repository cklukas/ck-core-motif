---
nav_exclude: true
---

# Drag-under animation allocation

Drag-under animation now checks its save-state allocation before initialization,
and `AnimateEnter` stops cleanly when that state cannot be created. The caller
still records the null state for the matching leave path, while no animation
GC, region, or window fields are dereferenced from a null record.

The focused source contract is checked by
`tests/automated/check-dragunder-animation-allocation.py`.
