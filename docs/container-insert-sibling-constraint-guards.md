---
nav_exclude: true
---
# Container insertion sibling-constraint guards

Container node insertion now skips siblings whose private constraint record is
unavailable while resequencing.  This keeps malformed or partially torn-down
siblings from crashing position ordering; valid sibling ordering remains
unchanged.

The source contract is checked by
`tests/automated/check-container-insert-sibling-constraint-guards.py`.
