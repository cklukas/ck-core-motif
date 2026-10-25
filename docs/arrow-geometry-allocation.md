---
nav_exclude: true
---

# Legacy arrow geometry allocation

The legacy arrow geometry builder now allocates its top, center, and bottom
rectangle arrays transactionally. If any allocation fails, all partial arrays
are released and the output pointers remain cleared after the existing cache
reset; successful geometry is unchanged.

The focused regression check is
`tests/automated/check-arrow-geometry-allocation.py`.
