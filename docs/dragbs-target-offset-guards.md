---
nav_exclude: true
---
# Drag target-table offset validation

The serialized target-table header stores `heap_offset` in a 32-bit field.
`WriteTargetsTable` now rejects a computed property size above `UINT32_MAX`
before assigning that field, preventing truncation of the serialized layout.
