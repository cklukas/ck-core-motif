---
nav_exclude: true
---
# Container ideal-geometry bounds

Container geometry negotiation accumulates header and child extents into
`Dimension` fields.  It also derives child `Position` values from outline
depth and indentation.  Those calculations now saturate instead of wrapping
when a large font, deeply nested tree, or unusually wide child exceeds the
legacy coordinate range.  This keeps negotiation monotonic and prevents a
wrapped value from producing a misleadingly small requested geometry.

The public resource types and ABI remain unchanged.  The focused source
contract is checked by
`tests/automated/check-container-ideal-geometry-bounds.py`.
