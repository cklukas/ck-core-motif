---
nav_exclude: true
---
# LabelG background-box bounds

LabelG now computes its background-box origin and inset width/height in signed
arithmetic before `LabelDrawBackground` clamps negative spans.  Collapsed
labels therefore avoid unsigned wraparound in the fill rectangle.

The source-level regression is `tests/automated/check-labelg-background-bounds.py`.
