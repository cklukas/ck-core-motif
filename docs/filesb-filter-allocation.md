---
nav_exclude: true
---
# FileSelectionBox filter allocation

The legacy FileSelectionBox filter field uses an empty string when no pattern
text is available.  If that fallback allocation fails, it now uses a
non-owned static empty string and avoids freeing it, preserving the empty
filter behavior without dereferencing or freeing invalid memory.

`tests/automated/check-filesb-filter-allocation.py` protects this contract.
