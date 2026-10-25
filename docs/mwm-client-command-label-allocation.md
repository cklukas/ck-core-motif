---
nav_exclude: true
---
# MWM client-command label allocation

Client-command parsing normalizes an input stream before storing it as a menu
label.  The final label copy is now checked; if it cannot be allocated, the
temporary parse buffer is released and parsing reports failure instead of
publishing an incomplete menu item.

`tests/automated/check-mwm-client-command-label-allocation.py` protects this
contract.
