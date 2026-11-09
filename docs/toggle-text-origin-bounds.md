---
nav_exclude: true
---
# ToggleButton text-origin bounds

ToggleButton computes text and accelerator origins from core size, indicator
space, margins, highlights, and shadows. Those sums can exceed the signed
legacy `Position` field even when every individual resource is valid. The
initial layout and later `ComputeSpace` alignment now retain wide arithmetic
and clamp only when storing the final text rectangle origin.

The focused regression is
`tests/automated/check-toggle-text-origin-bounds.py`.
