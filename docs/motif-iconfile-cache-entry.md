# Icon-file cache entry allocation

Icon path resolution treats name caching as optional.  If the cache entry
allocation fails, the resolved icon filename is now returned through the
normal path without dereferencing null cache storage.

The regression check is
`tests/automated/check-iconfile-entry-allocation.py`.
