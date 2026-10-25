# ToggleButton pixmap sizing dimensions

`SetToggleSize` now clears its temporary width and height outputs before each
pixmap metadata lookup. A failed or incomplete lookup therefore contributes
zero dimensions instead of reusing the dimensions of a preceding pixmap.
Valid pixmap metadata continues to participate in the existing maximum-size
calculation.

The focused regression is
`tests/automated/check-toggle-size-pixmap-dimensions.py`.
