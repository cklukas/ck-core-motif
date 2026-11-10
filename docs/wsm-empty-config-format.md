---
nav_exclude: true
---

# Empty WSM configuration formats

An empty global, window, or icon attribute list is a valid WSM configuration
format.  The reply decoder records it as a null list with a zero count without
performing a zero-byte allocation.  This keeps the peer-compatible empty
format valid on allocators that return null for a zero-size request.

Regression: `tests/automated/check-wsm-empty-collection-unpack.py`.
