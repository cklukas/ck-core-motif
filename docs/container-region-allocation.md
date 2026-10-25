---
nav_exclude: true
---
# Container region allocation

Removing a child from a `XmCELLS` Container temporarily allocates an X11
region.  Region allocation can fail under memory pressure; removal now skips
the geometric subtraction when either the temporary region or the persistent
cell region is unavailable, while continuing to release the child occupancy
state.  This prevents an allocation failure from becoming a null dereference.

`tests/automated/check-container-region-allocation.py` protects this failure
path.
