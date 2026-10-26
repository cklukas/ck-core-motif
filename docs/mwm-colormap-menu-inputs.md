# MWM colormap and menu getter inputs

`FindColormap` now returns `None` for a missing client record, and
`GetMwmMenuItems` returns `NULL` before accessing the client window. Normal
colormap lookup and `_MWM_MENU` parsing remain unchanged.

The focused source contract is covered by
`tests/automated/check-mwm-colormap-menu-inputs.py`.
