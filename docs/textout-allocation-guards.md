---
nav_exclude: true
---
# TextOut allocation guards

Text output initialization, Unicode paragraph caching, and baseline-array
retrieval now handle allocation failure explicitly.  Empty baseline results
are represented without allocating, while failed non-empty results return
failure instead of dereferencing null storage.

Line-wrap metadata is optional: if its allocation fails, layout computation
continues without the extra record.

Regression coverage: `tests/automated/check-textout-allocation-guards.py`.
