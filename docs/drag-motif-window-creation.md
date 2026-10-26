---
nav_exclude: true
---
# Drag protocol window creation

Motif keeps a small persistent, invisible X11 window for drag-and-drop target
properties.  Its creation path now checks `XCreateWindow` before mapping the
window or publishing its atom.  On failure it explicitly releases the server
grab, closes the auxiliary display, and returns `None`.
