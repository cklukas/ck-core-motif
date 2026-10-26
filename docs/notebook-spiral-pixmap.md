---
nav_exclude: true
---
# Notebook spiral pixmap creation

Notebook creates a server-side pixmap for its binding spiral and immediately
renders several arcs and lines into it.  The creation result is now checked
before rendering.  On failure the cached spiral is reset to
`XmUNSPECIFIED_PIXMAP`, so later expose handling can retry or omit the optional
decoration without issuing drawing requests against `None`.
