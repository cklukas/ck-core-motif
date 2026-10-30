---
nav_exclude: true
---
# Container child-list constraint guards

`XmContainerGetItemChildren` now treats a missing private constraint record as
an empty result and releases the application lock before returning.  Valid
parent and child lookups are unchanged, while teardown-time or malformed item
handles no longer reach `node_ptr` through a null record.

The source contract is checked by
`tests/automated/check-container-get-item-children-constraint-guards.py`.
