---
nav_exclude: true
---

# MWM colormap-reference array width

Per-window colormap reference lists grow as clients register references. MWM
tracks their indexes and removal position as `size_t`, checks the element count
against Xt's `Cardinal` width before allocating both grown and removal arrays,
and only replaces the published list after allocation succeeds. Oversized
reference lists follow the warning return path instead of overflowing a signed
index or wrapping pointer-array byte counts.
