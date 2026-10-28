# I18List column pixel-offset bounds

The helper that converts a column number to its pixel offset now accumulates
column widths in `long long` and clamps the historical `short` result.  Wide
lists therefore cannot wrap their starting pixel offset into a negative or
otherwise incorrect position during layout and hit testing.

The source-contract regression is
`tests/automated/check-i18-list-column-pixel-bounds.py`.
