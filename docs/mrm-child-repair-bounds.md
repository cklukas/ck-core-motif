---
nav_exclude: true
---
# Mrm child-repair bounds

`Idb__INX_FixNodeChildren` now rejects impossible node counts, heap offsets, and
free-space metadata before traversing child entries.  Its loop counter uses
`MrmCount`, avoiding truncation when processing large valid metadata values.

The source-level regression is
`tests/automated/check-mrm-child-repair-bounds.py`.
