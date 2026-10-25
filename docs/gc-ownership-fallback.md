---
nav_exclude: true
---
# Uncached image GC ownership

When the image GC cache cannot be initialized or a cache entry cannot be
allocated, `GetGCForPutImage` may still create a temporary GC. It now reports
that ownership to its caller, and the scaled-pixmap path releases the GC after
the image transfer. Cached GCs retain their existing lifecycle and are freed
with the cache.

The focused regression is `tests/automated/check-xm-gc-ownership.py`.
