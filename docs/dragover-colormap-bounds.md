---
nav_exclude: true
---
# Drag-over colormap bounds

The drag-over shell's colormap-window allocation remains bounded to Xt's
`Cardinal` width. Its teardown loop uses a distinct saved-colormap index,
avoiding shadowing of the active window-list index and making the two cleanup
passes unambiguous.

The focused regression is
`tests/automated/check-dragover-colormap-bounds.py`.
