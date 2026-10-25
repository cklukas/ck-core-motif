---
nav_exclude: true
---
# RowColumn popup-shell name allocation

RowColumn popup and pulldown creation now checks the combined prefix/name size
against host `size_t` and Xt's `Cardinal` limits, and returns `NULL` when the
temporary shell-name buffer cannot be represented or allocated, before
formatting or creating the shell. Successful menu creation remains unchanged.

`tests/automated/check-rowcolumn-popup-name-allocation.py` protects this
contract.
