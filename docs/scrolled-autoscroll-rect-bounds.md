---
nav_exclude: true
---
# ScrolledWindow auto-scroll rectangle bounds

Auto-scroll hotspots are represented by `XRectangle`, whose origins are signed
16-bit fields.  The old rectangle builders assigned work-area and scrollbar
coordinate differences directly, so large offsets could wrap the hotspot.  A
width/height conversion could also truncate an oversized extent.

Horizontal and vertical hotspot origins now use the bounded position helper,
and extents are clamped to the rectangle field range.  Normal autoscroll
behavior is unchanged; extreme layouts no longer place the hotspot at an
unrelated coordinate.  The contract is protected by
`tests/automated/check-scrolled-autoscroll-rect-bounds.py`.
