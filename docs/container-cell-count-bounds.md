# Container cell-count bounds

Container preferred-size calculation accumulates the number of cells needed
for grid and cell layouts in an `int` used by the integer square-root helper.
The accumulation now saturates at `INT_MAX` instead of overflowing when a large
set of children or a large child cell footprint exceeds that range.  Normal
small layouts retain the same result.

`tests/automated/check-container-cell-count-bounds.py` protects the saturating
arithmetic.
