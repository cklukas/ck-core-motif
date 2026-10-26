# Icon-cache flush path length bounds

`XmeFlushIconFileCache` now retains the path length returned by `strlen` as a
`size_t` while comparing and flushing cached directory entries. This avoids a
`Cardinal` narrowing wrap for unusually long invalidation paths.

The focused source regression is
`tests/automated/check-icon-cache-flush-length.py`.
