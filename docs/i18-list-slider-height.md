# I18List slider-height arithmetic

`ResizeSliders` now computes the vertical content height in signed wide
arithmetic before deriving rows per screen.  Small widgets no longer
underflow their unsigned `Dimension` height while subtracting title and
visual-style spacing, so scrollbar sizing remains stable.

The source-contract regression is
`tests/automated/check-i18-list-slider-height.py`.
