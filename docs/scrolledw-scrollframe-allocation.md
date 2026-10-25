---
nav_exclude: true
---
# ScrolledWindow scroll-frame allocation handling

ScrolledWindow’s scroll-frame trait initializer now checks its private data
allocation before initializing fields.  On failure it clears the initialized
flag and returns, allowing later queries to report an unavailable frame rather
than dereferencing null state.

The regression check is `tests/automated/check-scrolledw-scrollframe-allocation.py`.
