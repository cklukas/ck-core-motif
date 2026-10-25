---
nav_exclude: true
---
# Directory-entry list initialization

`_XmOSGetDirEntries` now checks its initial entry-list allocation. If the
allocation fails, it resets the allocation count, releases the Motif process
lock, and returns before any directory result can be written through a null
list.

`tests/automated/check-dir-entries-initial-allocation.py` protects this
contract.
