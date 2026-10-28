---
nav_exclude: true
---

# UIL database-header bounds

Compiled UIL database headers carry table counts and byte sizes as signed
integers, but those fields feed allocation and `fread` sizes. Database loading
now rejects negative values before entering the table dispatch, preventing a
corrupt database from turning a signed underflow into a huge allocation.

Regression: `tests/automated/check-uil-db-header-bounds.py`
