---
nav_exclude: true
---
# MultiList search-row geometry bounds

The optional `XmMultiList` search row derives child sizes, origins, and the
remaining text width from widget dimensions and border widths. Narrow
arithmetic could wrap child-plus-border sizes or turn a negative text origin
into a huge unsigned width.

The layout now uses wide intermediate arithmetic and clamps Position and
Dimension conversions. Ordinary search rows retain their existing geometry;
extreme dimensions remain bounded and deterministic.

The focused source regression is
`tests/automated/check-multilist-search-geometry-bounds.py`.
