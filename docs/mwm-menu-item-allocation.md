---
nav_exclude: true
---
# MWM menu-item allocation

MWM constructs menu items from resource strings.  Construction now accepts
null optional strings and checks each copied label, accelerator, and function
argument before publishing the item.  If any owned copy fails, the partially
initialized item is released and creation returns failure; valid items keep
the same fields and behavior.

`tests/automated/check-mwm-menu-item-allocation.py` protects this contract.
