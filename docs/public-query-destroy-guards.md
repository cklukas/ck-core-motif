---
nav_exclude: true
---

# Public query teardown guards

Public widget queries return their existing neutral values while Xt is
destroying the target: counts and generations return zero, indexes return
`-1`, booleans return `False`, and child/string handles return `NULL`.
AutoSuggestBox dismissal follows the same rule.  This prevents late
application callbacks from observing or forwarding partially disposed child
and model state.

`tests/automated/check-public-query-destroy-guards.py` records the affected
component APIs.
