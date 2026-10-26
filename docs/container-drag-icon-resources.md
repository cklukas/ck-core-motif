---
nav_exclude: true
---
# Container drag-icon resources

Container drag-and-drop creates two server-side pixmaps, a graphics context,
and a drag-icon widget for the temporary state indicator.  The construction
path now checks every X resource before drawing or publishing the per-display
context.  Partial resources are released when creation fails, so a server
resource exhaustion or an unrealized/invalid drawing target cannot turn into a
null graphics call or a half-published drag icon.
