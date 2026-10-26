# Text horizontal-page coordinate bounds

`MovePageLeft` and `MovePageRight` now clamp their hit-test y-coordinate at
zero before applying the font-ascent offset. This prevents the signed legacy
`Position` from wrapping when horizontal paging starts at the top edge of the
Text widget, while preserving the normal offset for all other rows.

The focused source contract is checked by
`tests/automated/check-text-horizontal-page-y-bounds.py`.
