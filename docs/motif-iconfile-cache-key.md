# Icon-file cache key allocation

The optional icon-name cache now checks the copied key string before filling
other cache fields.  If copying the key fails, the temporary cache record is
released and path resolution continues without caching.

The regression check is `tests/automated/check-iconfile-key-allocation.py`.
