---
nav_exclude: true
---
# Separator coordinate bounds

`XmeDrawSeparator` derives line centers and endpoints from widget origins,
dimensions, and margins before storing them in X11 `short` coordinates.
Direct narrowing could wrap oversized geometry and place a separator at an
unrelated coordinate.

The center and all single- and double-line endpoints now use the shared bounded
coordinate conversion. Normal separators retain their existing appearance;
extreme geometry remains within the X11 coordinate range.

The focused source regression is
`tests/automated/check-draw-separator-coordinate-bounds.py`.
