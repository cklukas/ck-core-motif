---
nav_exclude: true
---
# TreeView preorder scratch allocation

TreeView normalizes a caller-provided flat model into stable depth-first order
using two temporary arrays. Their byte count is now checked against host
`size_t` (where it is narrower) and Xt's `Cardinal` allocation range before
either array is allocated.
Unrepresentable public model counts fail without a wrapped allocation; valid
TreeView ordering and ownership behavior remain unchanged.

The focused source contract is covered by
`tests/automated/check-tree-view-preorder-allocation.py`.
