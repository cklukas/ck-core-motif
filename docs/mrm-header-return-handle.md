---
nav_exclude: true
---
# Mrm header RID return validation

`Idb__HDR_ReturnItem` now validates the stored record number and applies the
header/data-record offset limit before copying a resource handle to the
caller. Empty entries retain their existing `MrmNULL_DATA` behavior, while
partially corrupt RID entries return `MrmBAD_RECORD` safely. A supplied output
handle is initialized to zero before input validation; only the validated
stored handle is later published.
