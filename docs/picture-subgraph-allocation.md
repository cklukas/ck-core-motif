---
nav_exclude: true
---
# Picture subgraph allocation

Repeated `XmPicture` subgraphs use a temporary node table.  The table
allocation is now checked against both `size_t` and Xt `Cardinal` limits
before initialization; if memory is unavailable or unrepresentable on a
narrow target, the current graph end is returned unchanged instead of
dereferencing a null table.  Successful picture parsing and repetition
behavior are unchanged.

`tests/automated/check-picture-subgraph-allocation.py` protects this contract.
