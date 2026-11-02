---
nav_exclude: true
---
# ComboBox natural-size bounds

ComboBox now accumulates natural width and height in a wide integer while
combining text/list preferred sizes, borders, margins, and arrow metrics. The
result is explicitly saturated to Motif's `Dimension` range instead of
wrapping through a 16-bit intermediate. The default arrow-size calculation
uses the same boundary.

The focused regression is
`tests/automated/check-combobox-collapsed-geometry.py`.
