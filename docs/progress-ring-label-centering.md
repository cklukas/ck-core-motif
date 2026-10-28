---
nav_exclude: true
---

# ProgressRing label centering

ProgressRing now computes label Y offsets in signed arithmetic. If a label is
taller than the ring, its negative centering offset remains negative instead
of underflowing the unsigned `Dimension` subtraction and placing the label far
outside the widget.
