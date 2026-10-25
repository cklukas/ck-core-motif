---
nav_exclude: true
---
# MWM menu-button array bounds

`MakeMenu` builds a temporary button array from menu items that need greyed
or management-state handling.  It now checks the item count before multiplying
by the element size, rejecting an unrepresentable array without wrapping the
allocation byte count.  Normal menus retain the same button ordering and
resource behavior.

`tests/automated/check-mwm-menu-button-array-bounds.py` protects this contract.
