---
nav_exclude: true
---
# PNG `SCREEN_GAMMA` input

Motif's PNG decoder accepts the optional `SCREEN_GAMMA` environment variable
for applications that need to override the display exponent. The value is
accepted only when it is a complete, finite, strictly positive decimal number.
Malformed, overflowing, NaN, infinity, zero, and negative values are ignored
and the decoder keeps its platform-derived default exponent. This prevents a
process environment from sending invalid gamma values to libpng.
