# ToggleButton shadow origin bounds

`DrawToggleShadow` already rejects gadgets whose highlight consumes the
available drawing area, but its origin calculation added an unsigned
highlight thickness to the X11 `Position` before casting the result to `int`.
That could wrap an off-edge or negative origin before `XmeDrawShadows` saw it.

The helper now promotes both operands before addition. The fit checks,
dimensions, Motif resources, and public ABI remain unchanged. The regression
`tests/automated/check-togglebg-shadow-origins.py` keeps this helper on the
signed-arithmetic path.
