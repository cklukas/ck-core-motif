---
nav_exclude: true
---
# Mrm header RID allocation inputs

`Idb__HDR_NextRID` now rejects a null open-file handle or null output pointer
before reading or updating the next-RID state. Valid allocation behavior and
resource-ID layout remain unchanged.
