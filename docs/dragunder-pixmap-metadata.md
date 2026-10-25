---
nav_exclude: true
---
# DragUnder pixmap metadata guards

DragUnder now initializes metadata depth before configuring highlight, top
shadow, and bottom shadow GCs.  Tile or stipple state is added only for a
positive resolved depth; an unresolved pixmap leaves the solid color GC
configuration intact.

The focused regression is
`tests/automated/check-dragunder-pixmap-metadata.py`.
