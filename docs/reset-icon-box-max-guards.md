# Icon-box maximum-size reset guards

`ResetIconBoxMaxSize` now returns when its client or bulletin-board widget is
missing, avoiding invalid Xt queries and geometry writes.  Valid resets retain
the existing sizing and placement behavior.

`tests/automated/check-reset-icon-box-max-guards.py` protects the contract.
