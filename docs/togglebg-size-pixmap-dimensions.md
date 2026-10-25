---
nav_exclude: true
---
# ToggleButtonGadget pixmap sizing dimensions

`SetToggleSize` now clears its temporary width and height outputs before each
of the six selected and insensitive pixmap metadata lookups.  A failed or
incomplete lookup therefore contributes zero dimensions instead of reusing a
preceding pixmap's size.

The focused regression is
`tests/automated/check-togglebg-size-pixmap-dimensions.py`.
