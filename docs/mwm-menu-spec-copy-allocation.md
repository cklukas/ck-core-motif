---
nav_exclude: true
---
# MWM menu-spec copy allocation

Custom menus duplicate an existing menu specification before adding client
items.  The duplicate now handles a null source record and checks the copied
name before continuing.  If the name cannot be copied, the new record is
released and duplication fails cleanly; valid specifications retain the same
name and menu behavior.

`tests/automated/check-mwm-menu-spec-copy-allocation.py` protects this
contract.
