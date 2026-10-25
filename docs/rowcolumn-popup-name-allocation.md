---
nav_exclude: true
---
# RowColumn popup-shell name allocation

RowColumn popup and pulldown creation now returns `NULL` when its temporary
shell-name buffer cannot be allocated, before formatting or creating the
shell. Successful menu creation remains unchanged.

`tests/automated/check-rowcolumn-popup-name-allocation.py` protects this
contract.
