---
nav_exclude: true
---
# WSM target cache guards

Target validation and target-list construction now handle an unavailable WSM
display cache.  Validation returns `False`; target-list construction reports
an empty list through `len_ret` and returns `NULL`.  It also rejects a null
length output pointer and propagates target-array allocation failure safely.
This preserves normal target behavior while making failure cases safe.

`tests/automated/check-wsm-target-cache-guards.py` protects both callers.
