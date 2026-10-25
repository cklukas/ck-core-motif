---
nav_exclude: true
---
# List selection input validation

Item selection and deselection now reject null widgets before toolkit access,
and item-based operations reject null `XmString` values before list lookup.
Position selection behavior remains unchanged.

`tests/automated/check-list-selection-inputs.py` protects this contract.
