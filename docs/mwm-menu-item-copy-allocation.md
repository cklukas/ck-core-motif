---
nav_exclude: true
---
# MWM menu-item copy allocation

Custom menus duplicate their source menu items, including labels, accelerator
text, function arguments, and client-command names.  The copy now checks each
node and owned-string allocation, links only complete nodes, and releases the
current and previously copied items on failure.  Valid menu copies retain
their original ordering and ownership rules.

`tests/automated/check-mwm-menu-item-copy-allocation.py` protects this
contract.
