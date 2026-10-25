---
nav_exclude: true
---
# GeoUtils box-fitting allocation

`FitBoxesAveraging` temporarily sorts child geometry pointers while reducing a
row to fit its requested width.  It now returns without modifying the row when
there are no boxes, when the pointer-table size cannot be represented by
`size_t`, or when the temporary allocation fails.  This prevents a null-table
dereference and preserves the caller's existing geometry on allocation
failure.

Normal nonempty layouts retain the same sorting and width-fitting behavior.
The source-level regression is
`tests/automated/check-geoutils-fit-boxes.py`.
