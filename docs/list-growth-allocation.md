# List allocation failure handling

List insertion now treats the internal item-array resize as a transaction. It
checks integer, host `size_t`, and Xt `Cardinal` bounds, keeps the old array
when `XtRealloc` fails, and stops the insertion before internal rows or
selection state are updated.
The traversal highlight is changed only after the resize succeeds, keeping the
widget coherent under memory pressure while preserving the public
`XmListAdd*` API behavior.

`XmListGetMatchPos` also keeps its original result buffer when shrinking the
temporary match array cannot be satisfied, and rejects an initial allocation
failure. The caller still receives every match without a leaked buffer or
invalid null result.

The source-level regression check is `tests/automated/check-list-growth.py`.
