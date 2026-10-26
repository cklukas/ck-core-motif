# Text line-table export allocation

The semi-public line-table export now validates its line count before
computing the copy size and checks the Xt allocation result.  Invalid or
unavailable requests return `NULL` (and report zero lines after allocation
failure) without copying through null storage.

The focused source contract is checked by
`tests/automated/check-text-line-table-export.py`.
