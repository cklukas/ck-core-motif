---
nav_exclude: true
---
# TextOut allocation guards

Text output initialization, Unicode paragraph caching, and baseline-array
retrieval now handle allocation failure explicitly.  Empty baseline results
are represented without allocating, while failed non-empty results return
failure instead of dereferencing null storage.

Regression coverage: `tests/automated/check-textout-allocation-guards.py`.
