---
nav_exclude: true
---
# Divider geometry bounds

`XmDivider` now clamps preferred width after adding label spacing and margins,
preventing `Dimension` wraparound for large spacing resources. Label measurement
continues to use the active render table and safely falls back when unavailable.

The focused source check is
`tests/automated/check-divider-label-width.py`.
