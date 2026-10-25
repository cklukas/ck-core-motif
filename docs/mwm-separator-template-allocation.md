---
nav_exclude: true
---
# MWM separator-template allocation

Legacy CCI menu parsing optionally inserts separator-template items.  Template
and label allocation are now checked, and callers omit only the optional
separator when construction fails rather than linking a null node into the
menu list.  Actual menu items and valid separators retain their ordering and
behavior.

`tests/automated/check-mwm-separator-template-allocation.py` protects this
contract.
