---
nav_exclude: true
---

# TabStack tear-off callback guard

The optional tear-off callback now rejects a missing callback widget before
querying user data. Valid tear-off callbacks retain their existing behavior.

The focused source-contract check is
`python3 tests/automated/check-tabstack-tearoff-callback-guard.py`.
