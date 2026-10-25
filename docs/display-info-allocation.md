---
nav_exclude: true
---
# XmDisplay private-state allocation

`XmDisplay` initialization now checks its private `XmDisplayInfo` allocation
before initializing the state fields.  Allocation failure exits the
initializer safely, matching the existing screen-state guard; successful
display initialization is unchanged.

`tests/automated/check-display-info-allocation.py` protects this contract.
