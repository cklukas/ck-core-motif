---
nav_exclude: true
---
# Mrm B-tree child lookup bounds

`Idb__INX_GetBtreeRecord` now requires a node record, validates its count and
heap metadata, bounds the requested entry, and rejects empty child links before
asking the buffer manager to load a record.

The source-level regression is
`tests/automated/check-mrm-btree-record-bounds.py`.
