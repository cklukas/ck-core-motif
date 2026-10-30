---
nav_exclude: true
---
# FontSelector top-pane allocation

FontSelector now checks argument-list merging, the top paned container, and the
option toggle button before constructing dependent controls or registering its
callback.  A failed allocation returns cleanly without using a null parent or
button.
