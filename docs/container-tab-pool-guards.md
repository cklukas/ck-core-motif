---
nav_exclude: true
---
# Container dynamic tab-pool allocation

Dynamic detail-tab generation now checks both the temporary tab-array
allocation and every `XmTabCreate` result. The array size is checked against
host `size_t` and Xt's `Cardinal` width. If a tab cannot be created, the
already-created tabs and array are released and the Container reports that no
dynamic tab list is available. Existing per-call ownership and tab-list
compatibility remain unchanged.

`tests/automated/check-container-tab-pool-guards.py` protects this cleanup
path.
