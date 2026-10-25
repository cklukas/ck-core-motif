# Drag atom-table count validation

The `_MOTIF_DRAG_ATOMS` header stores `num_atoms` in a 16-bit field.
`WriteAtomsTable` now rejects an in-memory entry count above `UINT16_MAX`
instead of silently truncating it when publishing the cache.
