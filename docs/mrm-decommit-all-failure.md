# Mrm decommit-all failure retention

`Idb__BM_DecommitAll` now keeps a buffer attached to its `IDBFile` when
`Idb__BM_Decommit` cannot write it. The buffer remains marked modified, and
the first failure is returned after the remaining buffers are attempted. This
preserves the pending record for a retry and avoids reporting a failed close as
though its data had been released. Buffers that decommit successfully retain
the historical release behavior.
