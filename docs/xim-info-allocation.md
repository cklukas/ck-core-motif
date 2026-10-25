---
nav_exclude: true
---
# XIM info allocation

XIM setup now checks both the per-input-context record and its preedit buffer
before initialization.  If either allocation fails, the partially allocated
record is released and `create_xic_info` returns `NULL`, matching its caller
contract.  Successful input-method setup is unchanged.

`tests/automated/check-xim-info-allocation.py` protects this contract.
