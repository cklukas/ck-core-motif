# GeoUtils matrix allocation bounds

`_XmGeoMatrixAlloc` builds one contiguous allocation for row layouts, child
geometry records, and an optional extension.  Its size intermediates are now
`size_t`, with checked additions and multiplications before the request is
narrowed to Xt's `Cardinal` allocation size.  Failed or unrepresentable
requests return `NULL` before any pointer arithmetic or dereference.
The matching `_XmGeoMatrixFree` cleanup is NULL-safe, allowing allocation
failure paths to use the normal cleanup routine.
The SelectionBox, FileSelectionBox, and MessageBox matrix constructors also
propagate a failed allocation as `NULL` instead of dereferencing it.

Normal matrix layout and the private API signature are unchanged.  The
source-level regression is
`tests/automated/check-geoutils-matrix-allocation.py`.
