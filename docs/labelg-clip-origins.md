---
nav_exclude: true
---
# LabelG clip origins

LabelG narrow-layout clip origins now add highlight, shadow, and margin
insets in signed arithmetic before conversion to the X rectangle coordinate.
Labels positioned partly outside a parent therefore preserve their intended
off-edge clip origin.

The source-level regression is `tests/automated/check-labelg-clip-origins.py`.
