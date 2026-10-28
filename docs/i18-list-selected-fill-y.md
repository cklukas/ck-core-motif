# I18List selected-row fill Y advancement

The selected-row background loop in `DisplayList` now clamps each row-height
advance before drawing the next fill.  This closes the remaining direct
`Position` increment that could wrap on large row metrics.

The source-contract regression is
`tests/automated/check-i18-list-selected-fill-y.py`.
