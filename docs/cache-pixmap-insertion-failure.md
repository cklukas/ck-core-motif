# Cached pixmap insertion failure

`_XmCachePixmap` now publishes an entry transactionally across its pixmap and
pixmap-data tables. If either hash insertion fails, it removes the first
insertion and frees the copied name, color record and cache entry. The caller's
X11 pixmap remains owned by the caller, and the function reports failure.

The focused regression is
`tests/automated/check-xm-cache-pixmap-insertion.py`.
