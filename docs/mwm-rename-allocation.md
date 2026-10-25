---
nav_exclude: true
---
# MWM command rename allocation

Runtime command updates can rename an existing menu item.  The new label is
now copied before replacing the old one; if allocation fails, the old label
and visible menu remain intact.  Successful renames retain the existing
tear-off-safe update path and widget behavior.

`tests/automated/check-mwm-rename-allocation.py` protects this contract.
