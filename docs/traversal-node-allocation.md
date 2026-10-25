# Traversal-node pool allocation

Traversal graph node-pool creation and growth now allocate into temporary
pointers and publish new storage only after success. Size arithmetic checks
both host `size_t` and Xt `Cardinal` byte limits, with explicit Xt-sized byte
arguments, and graph construction stops cleanly when a node or control-graph
entry cannot be allocated instead of dereferencing a null pool.

The regression check is
`tests/automated/check-traversal-node-allocation.py`.
