---
nav_exclude: true
---
# Traversal row allocation

Traversal row construction now publishes replacement item and row arrays only
after their allocations succeed. Row growth checks both host `size_t` and Xt's
`Cardinal` byte limit before passing an explicit Xt-sized allocation. Failed
growth returns from sorting through an allocation-failure cleanup path,
releasing partial rows instead of losing the old item array or writing through
null storage.

`tests/automated/check-traversal-row-allocation.py` protects these paths.
