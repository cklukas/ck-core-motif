---
nav_exclude: true
---
# MWM menu-spec name allocation

MWM creates a menu specification before parsing its items.  The specification
name is copied from resource input, so allocation failure must abort creation
before the partially initialized record enters the menu list.  `MakeMenuSpec`
now rejects a null name and frees the record if copying the name fails; valid
menu specifications retain the same fields and behavior.

`tests/automated/check-mwm-menu-spec-allocation.py` protects this contract.
