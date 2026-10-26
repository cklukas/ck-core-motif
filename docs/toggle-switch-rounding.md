---
nav_exclude: true
---
# ToggleSwitch rounded drawing

ToggleSwitch tracks and knobs now use X11 filled arcs with a centered
rectangle, rather than a hand-rasterized row loop. The geometry is symmetric
for both ends and remains correct when the control dimensions change, avoiding
the one-pixel protrusions and uneven circles produced by the old approximation.

The focused source contract is checked by
`tests/automated/check-toggle-switch-rounding.py`.
