# I18List visibility-padding bounds

`MakePositionVisible` adds horizontal padding around a cell's right edge.
That padding is now applied in wide arithmetic and clamped before storing the
legacy `short` coordinate.  A cell near the representable edge can no longer
wrap when the visibility calculation adds its normal spacing.

The source-contract regression is
`tests/automated/check-i18-list-visibility-padding-bounds.py`.
