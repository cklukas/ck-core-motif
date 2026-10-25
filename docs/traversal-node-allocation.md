# Traversal-node pool allocation

Traversal graph node-pool creation and growth now allocate into temporary
pointers and publish new storage only after success.  Size arithmetic is
checked, and graph construction stops cleanly when a node or control-graph
entry cannot be allocated instead of dereferencing a null pool.

`tests/automated/check-traversal-node-allocation.py` protects these paths.

Pool trimming is transactional as well: failed shrink reallocations leave the
current head and capacity unchanged, and the trim condition avoids overflowing
the entry-count addition.
