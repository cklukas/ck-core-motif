---
nav_exclude: true
---
# XPM pixel-writer dimensions

The XPM file writer rejects zero-width or zero-height images before deriving
the final row count. This prevents unsigned underflow of `height - 1` from
turning malformed input into an enormous write loop.

The source contract is covered by
`tests/automated/check-xpm-write-pixels-dimensions.py`.
