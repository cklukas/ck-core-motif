# Paned regular sash positions

Paned now computes regular horizontal and vertical sash coordinates in signed
arithmetic before converting to `Position`.  Narrow panes and negative
off-edge placements no longer risk mixed signed and unsigned wraparound.

The source-level regression is `tests/automated/check-paned-sash-positions.py`.
