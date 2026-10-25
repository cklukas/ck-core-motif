---
nav_exclude: true
---
# MWM CCI argument allocation

The `f.cci` resource parser creates a modifier/entry record and copies the
entry text.  It now checks the record and every entry-string allocation, and
frees the record on malformed input or allocation failure.  Valid CCI
modifiers and entry text are unchanged.

`tests/automated/check-mwm-cci-argument-allocation.py` protects this contract.
