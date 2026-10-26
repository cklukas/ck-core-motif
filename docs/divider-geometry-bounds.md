---
nav_exclude: true
---
# Divider geometry bounds

`XmDivider` now clamps preferred width after adding label spacing and margins,
and clamps the thickness-based span used by both initialization and
`QueryGeometry`. This prevents `Dimension` wraparound for large spacing or
thickness resources. Label measurement continues to use the active render
table and safely falls back when unavailable.

The focused source check is
`tests/automated/check-divider-label-width.py`.
