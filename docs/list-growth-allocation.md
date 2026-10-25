# List allocation failure handling

List insertion now treats the internal item-array resize as a transaction. It
checks integer and `size_t` bounds, keeps the old array when `XtRealloc` fails,
and stops the insertion before internal rows or selection state are updated.
This keeps the widget coherent under memory pressure and preserves the public
`XmListAdd*` API behavior.

`XmListGetMatchPos` also keeps its original result buffer when shrinking the
temporary match array cannot be satisfied. The caller still receives every
match, with no leaked buffer or invalid null result.
