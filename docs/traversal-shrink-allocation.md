---
nav_exclude: true
---

# Traversal graph shrink bounds

Traversal graph pruning now validates the reduced node count against both
`size_t` and Xt's `Cardinal` allocation width before calling `XtRealloc`.
The existing transactional behavior remains intact when the shrink cannot be
represented or cannot be allocated.

Row-item and exclusion-tab list resizing uses the same dual-width checks, so
all traversal arrays reject byte counts that do not fit Xt's `Cardinal`.

The focused regression check is
`tests/automated/check-traversal-shrink-allocation.py`.
