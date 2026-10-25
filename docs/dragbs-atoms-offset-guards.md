# Drag atom-table offset validation

The serialized atom-table header stores `heap_offset` in a 32-bit field.
`WriteAtomsTable` now rejects a computed property size above `UINT32_MAX`
before assigning that field, preventing truncation of the serialized layout.
