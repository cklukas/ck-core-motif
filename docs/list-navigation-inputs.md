---
nav_exclude: true
---
# List selection and navigation input validation

`XmListDeselectAllItems`, `XmListSetPos`, and `XmListSetBottomPos` now reject
null widgets before application-context access or list-state reads. Valid
selection and navigation behavior remains unchanged.

`tests/automated/check-list-navigation-inputs.py` protects this contract.
