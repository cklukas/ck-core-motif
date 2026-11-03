---
nav_exclude: true
---
# Mrm record-buffer output initialization

The record-buffer acquisition and initialization entry points now set their
output pointer to `NULL` before doing further work. A failed allocation,
validation, or read therefore cannot leave a caller with a stale buffer from a
previous operation. `Idb__BM_GetBuffer` also rejects a non-positive pool size
before it searches or dereferences the pool, and clears a selected pool entry
from the output if record storage cannot be allocated. Successful calls retain
the existing buffer-selection behavior.
