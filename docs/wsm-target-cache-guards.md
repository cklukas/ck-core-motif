# WSM target cache guards

Target validation and target-list construction now handle an unavailable WSM
display cache.  Validation returns `False`; target-list construction reports
an empty list through `len_ret` and returns `NULL`.  This preserves normal
target behavior while allowing allocation failure to propagate safely.

`tests/automated/check-wsm-target-cache-guards.py` protects both callers.
