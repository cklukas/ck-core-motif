---
nav_exclude: true
---

# Legacy default-color cache allocation

`_XmGetDefaultColors` now grows its static default-color pointer array
transactionally. Integer overflow and allocation failure return `NULL` while
preserving the existing cache; successful color lookup and reuse are unchanged.

The focused regression check is
`tests/automated/check-default-colors-allocation.py`.
