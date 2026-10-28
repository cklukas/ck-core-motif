---
nav_exclude: true
---

# Xft drawable eviction

`_XmXftDrawEvict(Display *, Window)` clears and destroys an existing cached
draw without creating a replacement. Drawable owners can call it immediately
before releasing a pixmap or window, preventing stale Xft objects from being
returned if an XID is later reused.
