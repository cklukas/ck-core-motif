---
nav_exclude: true
---

# Chip geometry bounds

`XmChip` now clamps text, pixmap, and accessory geometry calculations to the
representable `Dimension` range. Long labels and unusually large pixmaps no
longer wrap into undersized geometry requests.

The focused regression check is
`tests/automated/check-chip-geometry-bounds.py`.
