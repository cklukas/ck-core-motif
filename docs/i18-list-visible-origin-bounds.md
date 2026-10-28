# I18List visible-origin bounds

`MakePositionVisible` derives the visible origin from the current horizontal
offset.  The adjustment now uses wide arithmetic and clamps to the legacy
`short` range, preventing an offset at the negative limit from wrapping the
origin and misclassifying a cell as already visible.

The source-contract regression is
`tests/automated/check-i18-list-visible-origin-bounds.py`.
