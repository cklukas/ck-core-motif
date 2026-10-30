---
nav_exclude: true
---
# Container reorder constraint guards

`XmContainerReorder` now validates the first item’s constraint and node record,
and skips list entries whose constraints are unavailable.  The reorder API
continues to preserve valid sibling ordering while malformed or teardown-time
items are ignored safely.

The source contract is checked by
`tests/automated/check-container-reorder-constraint-guards.py`.
