# I18List available-height arithmetic

`AdjustFirstRowAndCol` now computes the content height available for rows in
signed wide arithmetic and clamps negative results to zero.  Small widgets
can therefore not underflow their unsigned `Dimension` height and produce an
invalid rows-per-screen count.

The source-contract regression is
`tests/automated/check-i18-list-available-height.py`.
