# I18List total-width bounds

I18List retains its historical `short` width helper contract, but now sums
column widths in `long long` and saturates at `SHRT_MAX` before returning.
Without this guard, many wide columns could overflow the old cast and produce
a negative or otherwise incorrect content width, corrupting resize and
horizontal-scroll calculations.

The source-contract regression is
`tests/automated/check-i18-list-total-width-bounds.py`.
