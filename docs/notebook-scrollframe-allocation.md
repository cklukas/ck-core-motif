# Notebook scroll-frame allocation handling

Notebook’s scroll-frame trait initializer now checks its private
`XmScrollFrameData` allocation before initializing fields.  Allocation failure
leaves the trait uninitialized and returns cleanly, allowing the caller to
report an unavailable scroll frame instead of dereferencing null state.

The regression check is `tests/automated/check-notebook-scrollframe-allocation.py`.
