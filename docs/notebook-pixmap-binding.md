# Notebook pixmap binding metadata

Notebook pixmap bindings now distinguish a valid uncached metadata fetch from
an unresolved pixmap.  `XmeGetPixmapData` may return `False` after fetching a
valid pixmap into the cache, so binding drawing now validates the initialized
depth instead of using the Boolean cache-status result as an error signal.

The focused regression is `tests/automated/check-notebook-pixmap-binding.py`.
