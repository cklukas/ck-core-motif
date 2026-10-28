---
nav_exclude: true
---

# Xft drawable depth selection

The Xft draw cache now checks the drawable geometry before selecting a draw
implementation. Depth-one pixmaps use `XftDrawCreateBitmap`, avoiding the
lazy `BadMatch` that can occur when a default color visual is used for a
rotated-label bitmap. Other drawables use their realized window visual and
colormap when available, with the existing default-screen fallback retained.
