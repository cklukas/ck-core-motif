---
nav_exclude: true
---

# MWM backdrop-request path allocation

Workspace-manager backdrop-change requests copy the requested file path before
loading the pixmap. The copy now checks the path length against Xt's
`Cardinal` width before allocation; an unrepresentable or unavailable copy
continues through the existing no-op request path instead of wrapping the
allocation size.
