---
nav_exclude: true
---
# Container spatial-growth constraint guards

`RequestSpatialGrowth` now returns `False` when the requested child has no
Container constraint record.  Spatial growth uses the record's logical
coordinates for closest-cell placement; continuing without it could
dereference invalid state while computing the geometry request.  Valid child
growth remains unchanged.

The source contract is checked by
`tests/automated/check-container-spatial-growth-constraint-guards.py`.
