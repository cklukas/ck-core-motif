---
nav_exclude: true
---
# Drag target-list count validation

Each `_MOTIF_DRAG_TARGETS` list stores its atom count in a 16-bit wire field.
`WriteTargetsTable` now rejects a list whose in-memory count exceeds that
representation instead of silently truncating it and publishing a cache that
cannot be read consistently.
