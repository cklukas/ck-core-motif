---
nav_exclude: true
---

# MWM colormap-reference array width

Per-window colormap reference lists grow as clients register references. MWM
now checks the element count against Xt's `Cardinal` width before allocating
both grown and removal arrays. Oversized reference lists follow the warning
return path instead of wrapping pointer-array byte counts.
