# I18List pixmap centering

Selection highlighting now centers a row pixmap using signed wide arithmetic
before clamping the legacy short Y offset.  A pixmap taller than its row no
longer causes unsigned subtraction to underflow and place the image far from
the row.

The source-contract regression is
`tests/automated/check-i18-list-pixmap-centering.py`.
