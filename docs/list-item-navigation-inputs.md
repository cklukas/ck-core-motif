---
nav_exclude: true
---
# Item-based list navigation input validation

`XmListSetItem` and `XmListSetBottomItem` now reject null widgets or item
strings before toolkit access or list lookup. Valid navigation behavior is
unchanged.

`tests/automated/check-list-item-navigation-inputs.py` protects this contract.
