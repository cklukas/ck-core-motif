---
nav_exclude: true
---
# MWM quoted menu-name allocation

MWM resource parsing accepts a quoted label for client-command menu entries.
The parser now checks the measured name length and allocation result before
copying the label or writing its terminator.  An unrepresentable or
unallocatable label is ignored safely; valid labels and command matching are
unchanged.

`tests/automated/check-mwm-menu-name-allocation.py` protects this contract.
