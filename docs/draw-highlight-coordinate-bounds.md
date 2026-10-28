---
nav_exclude: true
---
# Highlight coordinate bounds

`XmeDrawHighlight` stores four rectangles using X11 signed 16-bit origins.
The right and bottom origins are derived from the widget origin and its
dimensions; direct casts could wrap for extreme geometry.

The derived origins now use the shared bounded coordinate conversion. Normal
highlight rendering is unchanged, while oversized geometry cannot wrap to an
unrelated location.

The focused source regression is
`tests/automated/check-draw-highlight-coordinate-bounds.py`.
