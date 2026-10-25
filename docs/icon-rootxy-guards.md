# Icon root-coordinate guards

`GetIconBoxIconRootXY` now initializes valid output pointers to `(0, 0)` and
returns safely for missing outputs, clients, screen data, or icon-box widgets.
Valid icon-box coordinate translation remains unchanged.

`tests/automated/check-icon-rootxy-guards.py` protects the contract.
