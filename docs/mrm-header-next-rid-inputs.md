---
nav_exclude: true
---
# Mrm header RID allocation inputs

`Idb__HDR_NextRID` rejects a null open-file handle or null output pointer
before reading or updating the next-RID state. Once its output is validated, it
is cleared before every header-state failure path. Valid allocation behavior
and resource-ID layout remain unchanged.
