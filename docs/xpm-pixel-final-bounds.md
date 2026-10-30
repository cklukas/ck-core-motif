---
nav_exclude: true
---
# XPM pixel final-row bounds

`WritePixels` now checks space for the closing quote of the final pixel row
before writing it, and checks the resulting size before updating the caller's
unsigned `used_size`.  Normal XPM output is unchanged; malformed or undersized
internal buffers now fail cleanly instead of advancing past their bound.

The regression check is
`tests/automated/check-xpm-pixel-final-bounds.py`.
