---
nav_exclude: true
---
# Mrm icon allocation bounds

UID icon mapping validates dimensions, source strides, destination byte counts,
and descriptor raster sizes before allocating image storage. Invalid or
unrepresentable icons return the existing Mrm failure result; ordinary icon
loading and XImage creation are unchanged.

The source-level regression is `tests/automated/check-mrm-icon-bounds.py`.
