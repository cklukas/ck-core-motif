---
nav_exclude: true
---
# WSM paired-property cleanup

The MWM WSM request unpacker consumes two wire properties for each match
attribute.  Allocation-failure cleanup now performs two bounded reads per
attribute instead of comparing an index against `num * 2`.  This avoids signed
integer overflow for hostile or corrupted list counts while preserving the
protocol layout and normal requests.

`tests/automated/check-wsm-paired-property-cleanup.py` protects the cleanup
loops.
