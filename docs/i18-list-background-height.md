# I18List background fill height

The full-list background fill now subtracts visual-style spacing only when
the widget height can contain it; otherwise it uses zero.  Small widgets no
longer underflow the unsigned `Dimension` and request an enormous fill
rectangle.

The source-contract regression is
`tests/automated/check-i18-list-background-height.py`.
