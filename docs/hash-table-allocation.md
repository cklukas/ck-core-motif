---
nav_exclude: true
---
# Hash-table allocation

The shared `_XmAllocHashTable` constructor now checks its table-record
allocation before initializing callbacks or bucket storage.  Allocation failure
returns `NULL` cleanly, allowing screen, image, and other caches to remain
uninitialized rather than dereferencing a missing table.

The focused regression is `tests/automated/check-hash-table-allocation.py`.
