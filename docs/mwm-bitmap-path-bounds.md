# Mwm bitmap path bounds

`BitmapPathName` now checks every home-directory, bitmap-directory and resolved
bitmap path before copying or appending it to the fixed `MAXWMPATH+1` result
buffer. Overlong paths return the original relative name or `NULL` rather than
truncating or producing an unterminated string. Normal lookup order and
short-path results are unchanged.

The focused source check is
`tests/automated/check-mwm-bitmap-path-bounds.py`.
