---
nav_exclude: true
---
# MWM backdrop color-set bounds

MWM validates a configured backdrop color-set ID and falls back to the
workspace default. The default is now validated too before indexing the pixel
set array, so an invalid or partially initialized workspace cannot turn
`colorSetId - 1` into an out-of-bounds access. Valid color-server behavior is
unchanged.

The source contract is checked by
`tests/automated/check-mwm-backdrop-colorset-bounds.py`.
