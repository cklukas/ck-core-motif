---
nav_exclude: true
---
# RowColumn menu resource allocation

RowColumn’s menu-accelerator and menu-post resource getters now expose a null
result when their returned copy cannot be allocated or its length cannot be
represented by Xt's `Cardinal` allocator, rather than copying into a null
buffer. Successful resource queries remain unchanged.

`set_values` now uses the same checked accelerator copy helper. If a changed
accelerator cannot be copied, the previous value remains active and the old
allocation is not released prematurely; successful replacements retain the
existing menu-tree update behavior.

`tests/automated/check-rowcolumn-resource-allocation.py` protects this
contract.
