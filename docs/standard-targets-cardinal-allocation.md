---
nav_exclude: true
---
# Standard target allocation width

`XmeStandardTargets` validates its target count against Xt's `Cardinal`
allocation limit and now passes the checked byte size with an explicit
`Cardinal` conversion. This makes the allocation contract clear on 32-bit and
64-bit builds.

The focused regressions are
`tests/automated/check-standard-targets-cardinal-allocation.py` and
`tests/automated/check-standard-targets-allocation.py`.
