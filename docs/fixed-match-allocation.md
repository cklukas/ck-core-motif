---
nav_exclude: true
---
# File-selector fixed-match allocation

File-selector filter construction now checks both temporary pattern buffers.
If the second buffer cannot be allocated, the first is released before the
helper returns `NULL`; successful filter normalization is unchanged.

`tests/automated/check-fixed-match-allocation.py` protects this contract.
