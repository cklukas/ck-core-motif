# Icon-file cache-table allocation fallback

Icon path resolution no longer assumes the optional icon-name hash table was
allocated successfully.  Lookup is skipped when the table is unavailable, and
newly built cache entries are released instead of being inserted into null
storage; path resolution continues normally.

The regression check is
`tests/automated/check-iconfile-hash-fallback.py`.
