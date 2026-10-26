---
nav_exclude: true
---
# Container spatial-size bounds

Spatial Container negotiation combines child positions, child extents, cell
counts, and margins into legacy `Dimension` values.  The calculation now uses
saturating arithmetic for those extents and cell-count products, preventing a
large icon grid or an unusually positioned child from wrapping into a smaller
requested size.  Existing layout policy and public resource types are
unchanged.

The focused source contract is checked by
`tests/automated/check-container-spatial-size-bounds.py`.
