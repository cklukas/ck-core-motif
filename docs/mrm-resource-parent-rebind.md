---
nav_exclude: true
---
# Mrm resource parent rebinding

`Idb__INX_FindResources` now reloads and validates the parent node at the start
of every node-entry iteration.  Recursive child traversal can turn over the
buffer cache, so reading the next entry only from the rebound node avoids stale
record pointers while preserving traversal order and filtering behavior.

The source-level regression is
`tests/automated/check-mrm-resource-parent-rebind.py`.
