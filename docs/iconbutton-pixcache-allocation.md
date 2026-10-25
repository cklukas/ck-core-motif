---
nav_exclude: true
---
# IconButton pixel-cache allocation handling

IconButton’s pixel-cache insertion now checks the cache entry, list creation,
and list-element insertion allocations.  Partial entries and a newly-created
empty list are released on failure, so cache updates do not dereference null
state or leak a failed insertion.

The regression check is `tests/automated/check-iconbutton-pixcache-allocation.py`.
