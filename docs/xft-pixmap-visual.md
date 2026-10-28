---
nav_exclude: true
---

# Xft pixmap visual selection

The Xft drawable helper uses `XGetGeometry` for information common to windows
and pixmaps. Depth-one pixmaps use the bitmap path; non-default-depth color
drawables select a visual matching both the drawable depth and the screen that
owns its root. Window-only attribute queries are never issued for arbitrary
drawables, and temporary visual-query results are released.
