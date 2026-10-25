---
nav_exclude: true
---
# Tree initialization allocation handling

Tree initialization now checks the resized top-node constraint record and the
child-operation list allocation before using either.  Allocation failure leaves
initialization safely incomplete rather than dereferencing null constraint or
list storage; successful hierarchy setup is unchanged.

The regression check is `tests/automated/check-tree-initialization-allocation.py`.
