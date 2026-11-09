# Scale metric bounds

Scale obtains value-trough text metrics from Xft/render tables or the legacy
font API, and derives tick/title padding and scrollbar extents from those
metrics.  The public and internal Motif geometry fields remain `Dimension`,
whose historical representation is an unsigned 16-bit value.

All of these calculations now remain in an integer metric until their final
geometry boundary.  `ScaleBoundedDimension` preserves representable positive
values, maps non-positive results to zero, and saturates an oversized result at
`USHRT_MAX`.  This avoids turning an unusually large font, DPI-derived extent,
or accumulated padding into a small wrapped Scale geometry.

The focused `check-scale-metric-bounds.py` regression check covers the value
trough height/ascent/descent/width paths, major leading/trailing padding, and
scrollbar width/height return boundaries.
