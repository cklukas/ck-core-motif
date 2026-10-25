---
nav_exclude: true
---
# List query and horizontal-position input validation

`XmListSetHorizPos` now rejects a null widget before application-context
access. `XmListPosSelected` also rejects empty-list and invalid last-item
states before resolving the special position zero to an internal index.

`tests/automated/check-list-query-inputs.py` protects this contract.
