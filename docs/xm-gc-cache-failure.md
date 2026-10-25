# Xm GC cache failure handling

`GetGCForPutImage` now handles lazy GC-table and `GCData` allocation failure
by creating an uncached GC. It creates a cache entry only after `XCreateGC`
succeeds, so failed server-side GC creation cannot publish a null resource.

The source-level regression is `tests/automated/check-xm-gc-cache-failure.py`.
