# I18List left-visibility movement bounds

When a cell begins left of the visible window, `MakePositionVisible` derives
a negative movement from the visible origin and cell start.  That movement is
now calculated in wide arithmetic and clamped before entering the historical
short-position helper, preventing large offsets from wrapping into a positive
movement.

The source-contract regression is
`tests/automated/check-i18-list-left-visibility-move-bounds.py`.
