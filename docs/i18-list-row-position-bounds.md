# I18List row-to-position bounds

`CvtRowColumnToPosition` now computes row Y coordinates in wide arithmetic
and clamps the legacy `short` output.  Large row offsets or row metrics can
no longer wrap the returned position and place the rendered cell incorrectly.

The source-contract regression is
`tests/automated/check-i18-list-row-position-bounds.py`.
