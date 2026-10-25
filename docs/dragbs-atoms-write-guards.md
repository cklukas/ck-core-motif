---
nav_exclude: true
---
# Drag atom-table serialization guards

`WriteAtomsTable` now rejects a non-empty cache with no entry array and checks
the heap allocation used for large serialized properties.  Failed or
incomplete internal state is reported and skipped before any serialization or
dereference occurs.
