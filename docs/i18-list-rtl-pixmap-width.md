# I18List RTL pixmap placement

Right-to-left pixmap placement now subtracts the pixmap width from the right
edge when computing its X destination.  The previous use of pixmap height
misplaced non-square images horizontally; Y centering continues to use the
height as intended.

The source-contract regression is
`tests/automated/check-i18-list-rtl-pixmap-width.py`.
