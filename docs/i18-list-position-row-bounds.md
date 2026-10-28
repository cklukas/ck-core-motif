# I18List position-to-row bounds

`CvtPositionToRowColumn` now keeps the Y coordinate in a wide local while
subtracting the header and spacing offsets.  The previous in-place mutation
of the `short` parameter could wrap for negative or edge coordinates before
row calculation; normal event and widget APIs remain unchanged.

The source-contract regression is
`tests/automated/check-i18-list-position-row-bounds.py`.
