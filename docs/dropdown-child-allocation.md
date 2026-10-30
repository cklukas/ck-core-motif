---
nav_exclude: true
---
# DropDown child allocation

`XmDropDown` creates label, text, and arrow children independently. Creation
now checks each child before resource updates, callback registration, and
translation installation. Geometry queries return zero size for a missing
child, and placement configures only children that exist. Normal DropDown
behavior and public child accessors are unchanged.

The contract is protected by
`tests/automated/check-dropdown-child-allocation.py`.
