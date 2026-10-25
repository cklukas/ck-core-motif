---
nav_exclude: true
---
# IconG clip Y origins

IconG applies highlight and margin dimensions to the gadget's Y position when
setting pixmap-mask and text clip rectangles. Those dimensions are unsigned,
so the old expression could wrap before reaching X when an icon was positioned
near an edge or had a narrow layout.

The four affected clip paths now promote each operand to `int` before the
addition and convert the bounded result at the `XRectangle` boundary. Existing
horizontal clipping and public behavior remain unchanged. The regression
`tests/automated/check-icong-clip-origins.py` covers all four paths.
