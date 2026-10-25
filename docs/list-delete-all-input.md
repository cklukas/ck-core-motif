---
nav_exclude: true
---
# List delete-all input validation

`XmListDeleteAllItems` now rejects a null widget before application-context
access or list-state reads. Valid list clearing behavior is unchanged.

`tests/automated/check-list-delete-all-input.py` protects this contract.
