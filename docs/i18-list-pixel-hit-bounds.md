# I18List pixel hit-test bounds

Pixel-to-column hit testing now accumulates column positions in `long long`.
Previously, the `short` accumulator could wrap while traversing wide columns,
causing selection and pointer hit testing to identify the wrong column.  The
existing `short` event-coordinate and column-index interfaces remain intact.

The source-contract regression is
`tests/automated/check-i18-list-pixel-hit-bounds.py`.
