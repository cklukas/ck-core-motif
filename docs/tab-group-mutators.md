---
nav_exclude: true
---

# Tab-group mutator input validation

`XmAddTabGroup` and `XmRemoveTabGroup` now return safely when passed a missing
widget instead of forwarding it to `XtSetValues`. Valid tab-group resource
updates remain unchanged.

The focused regression check is
`tests/automated/check-tab-group-mutators.py`.
