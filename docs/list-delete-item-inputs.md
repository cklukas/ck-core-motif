---
nav_exclude: true
---
# List item deletion input validation

`XmListDeleteItem` now rejects a null widget or null `XmString` before entering
the application context or searching the list. Valid item deletion remains
unchanged.

`tests/automated/check-list-delete-item-inputs.py` protects this contract.
