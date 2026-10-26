---
nav_exclude: true
---
# Text search conversion allocations

Forward and backward Text searches now check converted search-string sizes
against Xt's allocation width and handle allocation failure before converting
or dereferencing the temporary 16-bit and wide-character buffers.

Regression coverage: `tests/automated/check-text-find-search-allocation.py`.
