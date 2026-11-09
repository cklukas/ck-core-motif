---
nav_exclude: true
---
# Tree initialization allocation handling

Tree initialization now checks the resized top-node constraint record and the
child-operation list allocation before using either. Allocation failure raises
an Xt error rather than leaving a partially initialized widget whose later
callbacks could dereference null constraint or list storage; successful
hierarchy setup is unchanged.

The regression check is `tests/automated/check-tree-initialization-allocation.py`.
