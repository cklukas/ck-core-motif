---
nav_exclude: true
---
# Mrm RID-map handle bounds

RID-map entries store pointers to data or header records. `Idb__RID_EnterItem`
and `Idb__RID_ReturnItem` now reject non-positive record numbers and apply the
correct header/data-record offset bound before storing or returning a handle.
Empty RID entries retain their existing null-result behavior.
