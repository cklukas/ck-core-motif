---
nav_exclude: true
---
# Mrm index list allocation failures

`Idb__INX_FindResources` builds its result through `UrmPlistAppendString`.
Both leaf and node traversal paths now propagate an append failure instead of
returning `MrmSUCCESS` with an incomplete resource list.
