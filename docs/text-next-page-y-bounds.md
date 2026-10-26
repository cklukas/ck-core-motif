# Text next-page coordinate bounds

`MoveNextPage` now clamps both branches of its hit-test y-coordinate. The
top-margin adjustment is saturated before it is stored in the signed
`Position`, and a coordinate smaller than the font ascent resolves to zero
instead of wrapping negative. Normal page movement retains the existing
font-ascent offset.

The focused source contract is checked by
`tests/automated/check-text-next-page-y-bounds.py`.
