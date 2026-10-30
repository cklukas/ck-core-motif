---
nav_exclude: true
---
# FontSelector middle-pane allocation

FontSelector now checks merged arguments and both middle-area paned containers
before constructing dependent family, size, spacing, and choice controls.
Allocation failure returns cleanly without using a null parent widget.
