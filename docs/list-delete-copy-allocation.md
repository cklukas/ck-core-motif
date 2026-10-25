---
nav_exclude: true
---
# List deletion copy allocation

`XmListDeleteItems` now checks the temporary `XmString` copy-array size for
overflow and handles allocation failure before copying or deleting items. The
application lock is released on both failure paths, preserving normal list
deletion behavior while avoiding a null dereference.

`tests/automated/check-list-delete-copy-allocation.py` protects this contract.
