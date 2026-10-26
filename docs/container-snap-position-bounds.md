---
nav_exclude: true
---
# Container snap-position bounds

Container snapping and centering derives signed `XPoint` coordinates from cell
counts, cell dimensions, and child extents.  The calculations now use signed
saturating adjustments, so oversized cells cannot wrap a coordinate during
SNAP, NONE, or CENTER placement.  Normal layouts and the public resource/ABI
contract are unchanged.

The focused source contract is checked by
`tests/automated/check-container-snap-position-bounds.py`.
