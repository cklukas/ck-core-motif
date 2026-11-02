---
nav_exclude: true
---
# DropDown frame drawing bounds

The modern-style DropDown frame now calculates its child extents in a wide
intermediate type. Its drawing origin and dimensions are clamped explicitly
when they cross Motif's legacy `Position` and `Dimension` interfaces, so a
large child or margin cannot wrap the frame around the control.

The focused regression is `tests/automated/check-dropdown-frame-bounds.py`.
