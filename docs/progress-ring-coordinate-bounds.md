---
nav_exclude: true
---

# ProgressRing coordinate bounds

ProgressRing now keeps its arc center in signed `int` arithmetic until the X
arc is drawn.  Text and label positions are clamped before conversion to
Motif's signed `Position` type.  Large spacing or DPI-derived dimensions can
therefore be clipped safely instead of wrapping to an unrelated screen
coordinate.

`tests/automated/check-progress-ring-coordinate-bounds.py` protects this
coordinate contract.
