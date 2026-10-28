---
nav_exclude: true
---

# List draw-origin bounds

List now clamps the combined BaseX/XOrigin draw origin in both left-to-right
and right-to-left layouts.  Large horizontal scroll offsets therefore cannot
wrap the row origin through Motif's signed `Position` type.

`tests/automated/check-list-draw-origin-bounds.py` protects this contract.
