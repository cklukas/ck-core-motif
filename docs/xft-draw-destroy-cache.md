---
nav_exclude: true
---

# Xft draw-cache teardown

`_XmXftDrawDestroy` now destroys the object owned by the matching cache entry,
not the caller’s pointer. The cache slot is cleared first, and null cached
objects are skipped, making repeated or delayed teardown safe when rotated
label pixmaps are released.
