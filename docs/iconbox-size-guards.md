---
nav_exclude: true
---
# Icon-box size-check guards

`CheckIconBoxSize` now returns `False` when its icon-box widget, client data,
or cell increments are unavailable, avoiding invalid Xt queries and division
by zero.  Valid resize checks remain unchanged.

`tests/automated/check-iconbox-size-guards.py` protects the contract.
