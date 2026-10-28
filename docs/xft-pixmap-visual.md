---
nav_exclude: true
---

# Xft pixmap visual selection

The Xft drawable helper distinguishes windows from pixmaps using geometry.
Window drawables use their realized visual and colormap; depth-one pixmaps use
the bitmap path; and non-default-depth color pixmaps select a visual matching
their depth. Window-only attribute queries are never issued for pixmaps, and
temporary visual-query results are released.
