# MWM transient-window cache bounds

The transient restacking and lowest-window helpers now check their `count + 5`
growth against both `int` and Xt/WM allocation widths before multiplying by
`sizeof(Window)`. They publish the resized cache only after `WmMalloc()`
succeeds, preserving the prior cache on allocation failure.

The source contract is covered by
`tests/automated/check-mwm-transient-window-bounds.py`.
