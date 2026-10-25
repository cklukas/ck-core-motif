---
nav_exclude: true
---
# RowColumn menu resource allocation

RowColumn’s menu-accelerator and menu-post resource getters now expose a null
result when their returned copy cannot be allocated or its length cannot be
represented by Xt's `Cardinal` allocator, rather than copying into a null
buffer. Successful resource queries remain unchanged.

`tests/automated/check-rowcolumn-resource-allocation.py` protects this
contract.
