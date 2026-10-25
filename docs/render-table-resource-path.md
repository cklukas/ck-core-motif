---
nav_exclude: true
---
# Render resource paths

Rendition resource lookup now bounds the widget-ancestor path and reserves
space for the resource name, optional rendition tag, and terminating quarks.
Very deep widget hierarchies no longer overrun the fixed lookup buffers.
