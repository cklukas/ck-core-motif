# Text previous-page coordinate bounds

`MovePreviousPage` now clamps its target y-coordinate at zero before subtracting
the font ascent. At the top edge of a Text widget this prevents the signed
`Position` coordinate from wrapping to a large positive value and moving the
cursor to an unrelated line. Normal page movement retains the historical
font-ascent adjustment.

The focused source contract is checked by
`tests/automated/check-text-previous-page-y-bounds.py`.
