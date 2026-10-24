# Pixmap-cache geometry status

When dimensions were omitted, `_XmCachePixmap` ignored the status returned by
`XGetGeometry`.  Invalid server-side pixmap handles could therefore leave
uninitialized width, height, and depth values in the cache.

The query is now required to succeed before cache allocation.  Failed geometry
lookup returns `False` without creating an entry.

`tests/automated/check-cache-geometry-status.py` protects this contract.
