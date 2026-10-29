---
nav_exclude: true
---

# Scale label placement

Scale tick labels are placed from the scrollbar’s slider area.  The available
interval can be smaller than a label, especially during a narrow or short
allocation.  Placement now keeps interval endpoints and the single-label
centering difference in signed arithmetic for horizontal and vertical scales;
an oversized label is clipped near the control instead of wrapping to an
extreme coordinate.

The source contract is checked by
`tests/automated/check-scale-label-placement.py`.
