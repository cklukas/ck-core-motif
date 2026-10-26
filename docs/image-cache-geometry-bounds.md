# Cached pixmap geometry bounds

`_XmCachePixmap` now rejects X server geometry that cannot fit the cache
record's public `Dimension` width and height fields or its integer depth field.
This prevents large queried pixmaps from being silently narrowed before
metadata consumers use them.

The focused source contract is covered by
`tests/automated/check-cache-geometry-status.py`.
