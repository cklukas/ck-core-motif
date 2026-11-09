---
nav_exclude: true
---
# ToggleButtonGadget pixmap redisplay bounds

ToggleButtonGadget temporarily maps its selected X pixmap dimensions into the
LabelGadget text rectangle during redisplay. The pixmap values are wider than
the legacy rectangle coordinate and dimension fields, so the values are now
explicitly clamped at that boundary. This preserves ordinary pixmap layout
while avoiding wrapped redraw geometry for very large pixmaps.

The focused regression is
`tests/automated/check-togglebg-pixmap-geometry-bounds.py`.
