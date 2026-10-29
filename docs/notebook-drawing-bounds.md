---
nav_exclude: true
---

# Notebook drawing bounds

Notebook binding and frame drawing subtracts borders from the current frame
allocation.  During a transient tiny allocation those signed extents can be
zero or negative; keeping them in `Dimension` would wrap them into enormous
X11 rectangles.  The drawing paths now use signed extents and return without
drawing when no positive area remains.

The source contract is checked by
`tests/automated/check-notebook-drawing-bounds.py`.
