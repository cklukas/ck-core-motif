# Mrm header RID handle validation

Resource-ID entries in the header store a record number and an entry offset.
`Idb__HDR_EnterItem` now rejects non-positive record numbers and applies the
appropriate offset limit for header versus data records before writing the RID
table. Valid handles produced by Mrm remain unchanged.
