---
nav_exclude: true
---

# WSM window-data allocation width

WSM decodes a bit mask into a variable-length `WSMWinData` array. The decoded
count is now checked against Xt's `Cardinal` allocation width before the
element-size multiplication.  A malformed negative configuration attribute
count is rejected consistently by request packing, sizing, and data packing
before calculating the bit-mask size.  Invalid or unrepresentable counts
follow the existing allocation-failure cleanup path, preserving stream cleanup
and the public zero-count result.

The regression is covered by
`tests/automated/check-wsm-win-data-allocation-width.py`.
