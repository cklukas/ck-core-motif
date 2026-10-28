---
nav_exclude: true
---
# Shared shadow coordinate bounds

The shared shadow renderer stores endpoints in X11 `short` coordinates. Its
endpoint calculations combine widget origins, dimensions, and shadow offsets;
direct casts could wrap an oversized result into an unrelated screen location.

`DrawSimpleShadow` now clamps every computed endpoint to the representable X11
coordinate range before populating `XSegment`. Ordinary widget geometry is
unchanged, while extreme geometry remains bounded and deterministic.

The focused source regression is
`tests/automated/check-draw-shadow-coordinate-bounds.py`.
