# Mrm record-type validation

`Idb__BM_InitRecord` updates the per-file record-type counters after creating a
record. Those counters have a fixed `IDBrtMin` through `IDBrtMax` range. The
initializer now rejects values outside that range before acquiring or mutating
a buffer, preventing an invalid type from indexing the counter array and
leaving a partially initialized record behind.
