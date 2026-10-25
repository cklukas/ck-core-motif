---
nav_exclude: true
---

# Icon-label geometry bounds

`XmIconLabel` now clamps text measurements and combined icon-plus-label widths
to the representable `Dimension` range. Long labels no longer wrap around to a
small geometry request, while normal labels retain the existing sizing.

The focused regression check is
`tests/automated/check-icon-label-geometry-bounds.py`.
