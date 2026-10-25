---
nav_exclude: true
---

# Child geometry-list allocation

`_XmGetKidGeo` now validates the parent pointer, child-count arithmetic, and
`XtMalloc` result before populating the geometry list. Allocation failure or
an unrepresentable list size returns `NULL` instead of dereferencing partial
state. The checked byte count is passed explicitly as an Xt `Cardinal`.

The focused regression check is
`tests/automated/check-geo-kid-allocation.py`.
