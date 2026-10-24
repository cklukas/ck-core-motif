# Mrm buffer cache-hit validation

`Idb__BM_GetRecord` scans the buffer pool before loading a record from disk. A
cache entry is considered a hit only when it belongs to the requested file and
contains record storage. The storage check precedes access to the record
header, so a partially initialized or reclaimed entry is treated as a miss
instead of being dereferenced.

This preserves the existing cache behavior for valid entries while making the
scan safe during cleanup and recovery paths.
