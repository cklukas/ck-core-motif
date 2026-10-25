---
nav_exclude: true
---
# MWM menu-exclusion copy allocation

Custom menus duplicate the exclusion list from their source menu.  The copy
now checks each node and command-string allocation, links only fully
initialized nodes, and releases the partial list on failure.  A failed copy
therefore aborts custom-menu construction cleanly instead of dereferencing a
null node or retaining leaked partial state.  Successful copies preserve list
order and matching behavior.

`tests/automated/check-mwm-exclusion-copy-allocation.py` protects this
contract.
