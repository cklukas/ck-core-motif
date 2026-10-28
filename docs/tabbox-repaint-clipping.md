---
nav_exclude: true
---

# TabBox partial repaint and bitmap lifetime

TabBox label rendering can repaint a complete tab even when Xt supplies only a
small damage region. Redisplay now expands damage to intersected tab bounds,
restores the TabBox clip before each tab draw, and releases the temporary
region on every exit path. Cached Xft pictures are evicted before the reusable
label bitmap is freed, preventing stale drawable references during resize,
rotation, and destruction.
