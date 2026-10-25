---
nav_exclude: true
---
# Scaled image row offset types

The fast 8-bit scaling path forms source and destination row addresses for
each pixel.  Those products now use `size_t` before multiplication, matching
the checked allocation sizes and preventing 32-bit unsigned wraparound for
large valid strides or malformed dimensions.

`tests/automated/check-image-row-offset-types.py` protects these offset
contracts.
