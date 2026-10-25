---
nav_exclude: true
---
# Xm pixmap-cache allocation failure handling

`_XmCachePixmap` now checks cache-table initialization and every heap
allocation before publishing a cache entry. If the table, entry, color data,
or copied image name cannot be allocated, it frees the partial entry and
returns `False` without touching the hash tables.

This prevents memory pressure from becoming a null dereference or a partially
owned cache record. Successful cache insertion is unchanged. The source-level
regression is `tests/automated/check-xm-cache-allocation-failure.py`.
